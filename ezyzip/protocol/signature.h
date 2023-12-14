#include <iostream>
#include <vector>
#include <stdexcept>
#include "../cryptopp/eccrypto.h"
#include "../cryptopp/oids.h"
#include "../cryptopp/integer.h"
#include "../cryptopp/keccak.h"
#include "../cryptopp/hex.h"

class ECDSASignature {
public:
    CryptoPP::Integer r;
    CryptoPP::Integer s;

    ECDSASignature(const CryptoPP::Integer& r, const CryptoPP::Integer& s) : r(r), s(s) {}

    bool isCanonical() const {
        return s <= HALF_CURVE_ORDER;
    }

    ECDSASignature toCanonicalised() const {
        return !isCanonical() ? ECDSASignature(r, CURVE.GetGroupParameters().GetSubgroupOrder() - s) : *this;
    }
};

class Signature {
public:
    static const CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP>& CURVE;
    static const CryptoPP::Integer HALF_CURVE_ORDER;

    static ECDSASignature signMessage(const std::vector<uint8_t>& message, const CryptoPP::Integer& privateKey);
    static std::vector<uint8_t> signMessageBytes(const std::vector<uint8_t>& message, const CryptoPP::Integer& privateKey);
    static CryptoPP::Integer recoverFromSignature(int recId, const ECDSASignature& sig, const std::vector<uint8_t>& message);
    static CryptoPP::Integer publicKeyFromPrivate(const CryptoPP::Integer& privKey);
    static CryptoPP::ECPPoint publicPointFromPrivate(const CryptoPP::Integer& privKey);
    static std::vector<uint8_t> toBytesPadded(const CryptoPP::Integer& value, size_t length);
    static void verifyPrecondition(bool assertionResult, const std::string& errorMessage);

private:
    static CryptoPP::ECPPoint decompressKey(const CryptoPP::Integer& xBN, bool yBit);
    static std::vector<uint8_t> sha3(const std::vector<uint8_t>& input);
    static std::vector<uint8_t> concatenate(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);
};

const CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP>& Signature::CURVE = CryptoPP::ASN1::secp256k1();
const CryptoPP::Integer Signature::HALF_CURVE_ORDER = CURVE.GetGroupParameters().GetSubgroupOrder() >> 1;

ECDSASignature Signature::signMessage(const std::vector<uint8_t>& message, const CryptoPP::Integer& privateKey) {
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey privKey;
    privKey.Initialize(rng, CURVE, privateKey);

    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer signer(privKey);

    CryptoPP::SecByteBlock signature(signer.MaxSignatureLength());
    size_t signatureLength;

    signatureLength = signer.SignMessage(rng, message.data(), message.size(), signature);

    // Extract r and s from the signature
    CryptoPP::Integer r, s;
    r.Decode(signature, 32);
    s.Decode(signature + 32, 32);

    ECDSASignature ecdsaSignature(r, s);
    return ecdsaSignature;
}

CryptoPP::Integer Signature::recoverFromSignature(int recId, const ECDSASignature& sig, const std::vector<uint8_t>& message) {
    verifyPrecondition(recId >= 0, "recId must be positive");
    verifyPrecondition(sig.r.IsPositive(), "r must be positive");
    verifyPrecondition(sig.s.IsPositive(), "s must be positive");
    verifyPrecondition(!message.empty(), "message cannot be null");

    CryptoPP::Integer n = CURVE.GetGroupParameters().GetSubgroupOrder();
    CryptoPP::Integer i = recId / 2;
    CryptoPP::Integer x = sig.r + i * n;
    CryptoPP::Integer prime = CURVE.GetField().MaxElement();

    if (x >= prime) {
        return CryptoPP::Integer::Zero();
    } else {
        CryptoPP::ECPPoint R = decompressKey(x, recId % 2 == 1);

        if (!(R * n).IsZero()) {
            return CryptoPP::Integer::Zero();
        } else {
            CryptoPP::Integer e(reinterpret_cast<const byte*>(message.data()), message.size());
            CryptoPP::Integer eInv = -e % n;
            CryptoPP::Integer rInv = sig.r.InverseMod(n);
            CryptoPP::Integer srInv = rInv * sig.s % n;
            CryptoPP::Integer eInvrInv = rInv * eInv % n;
            CryptoPP::ECPPoint q = CryptoPP::ECP::PointMultiply(CURVE.GetBasePrecomputation(), eInvrInv, srInv);
            CryptoPP::SecByteBlock qBytes(q.x.ByteCount() + 1);
            q.x.Encode(qBytes, qBytes.size(), true);

            // Skip the first byte (prefix)
            return CryptoPP::Integer(qBytes + 1, qBytes.size() - 1);
        }
    }
}

CryptoPP::ECPPoint Signature::decompressKey(const CryptoPP::Integer& xBN, bool yBit) {
    CryptoPP::ECPPoint point;
    point.x = xBN;
    point.identity = false;

    if (!CryptoPP::ECP::DecompressPoint(CURVE.GetCurve(), point, yBit)) {
        throw std::runtime_error("Failed to decompress key");
    }

    return point;
}

CryptoPP::Integer Signature::publicKeyFromPrivate(const CryptoPP::Integer& privKey) {
    return publicPointFromPrivate(privKey).x;
}

CryptoPP::ECPPoint Signature::publicPointFromPrivate(const CryptoPP::Integer& privKey) {
    CryptoPP::ECPPoint point = CURVE.GetCurve().Multiply(CURVE.GetBasePrecomputation(), privKey);
    point.identity = false;
    return point;
}

std::vector<uint8_t> Signature::toBytesPadded(const CryptoPP::Integer& value, size_t length) {
    std::vector<uint8_t> result(length, 0);
    size_t byteCount = value.MinEncodedSize();

    if (byteCount > length) {
        throw std::runtime_error("Input is too large to put in byte array of size " + std::to_string(length));
    } else {
        value.Encode(result.data() + length - byteCount, byteCount);
        return result;
    }
}

std::vector<uint8_t> Signature::sha3(const std::vector<uint8_t>& input) {
    CryptoPP::SHA3_256 sha3;
    sha3.Update(input.data(), input.size());
    std::vector<uint8_t> result(sha3.DigestSize());
    sha3.Final(result.data());
    return result;
}

std::vector<uint8_t> Signature::concatenate(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    std::vector<uint8_t> result(a);
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

// int main() {
//     // Example usage
//     CryptoPP::Integer privateKey("1234567890123456789012345678901234567890123456789012345678901234");
//     std::vector<uint8_t> message = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};

//     try {
//         ECDSASignature signature = Signature::signMessage(message, privateKey);
//         std::cout << "r: " << signature.r << std::endl;
//         std::cout << "s: " << signature.s << std::endl;

//         // Example verification
//         CryptoPP::Integer recoveredKey = Signature::recoverFromSignature(0, signature, message);
//         std::cout << "Recovered Key: " << recoveredKey << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Exception: " << e.what() << std::endl;
//     }

//     return 0;
// }
