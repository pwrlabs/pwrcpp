#include <iostream>
#include <string>
#include "cryptopp/osrng.h"
#include "cryptopp/eccrypto.h"
#include "cryptopp/sha.h"
#include "cryptopp/integer.h"
#include "cryptopp/hex.h"
#include "cryptopp/secblock.h"
#pragma once
class PWRWallet {
private:
    CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey privateKey;

public:
    PWRWallet(const std::string& privateKeyHex) {
        setPrivateKey(privateKeyHex);
    }

    ~PWRWallet() {
        // Crypto++ handles memory management internally
    }

    std::string getAddress() const {
        CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey publicKey;
        privateKey.MakePublicKey(publicKey);

        CryptoPP::ECP::Point point = publicKey.GetPublicElement();
        CryptoPP::Integer x = point.x;
        CryptoPP::Integer y = point.y;

        return SHA(x, y);
    }

private:
    void setPrivateKey(const std::string& privateKeyHex) {
        CryptoPP::AutoSeededRandomPool rng;

        // Decode the hex string to Integer
        CryptoPP::Integer privateKeyInt(privateKeyHex.c_str());

        // Use the predefined secp256r1 curve
        CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Domain domain(CryptoPP::ASN1::secp256r1());

        // Initialize the private key
        privateKey.Initialize(domain, rng);
        privateKey.SetPrivateExponent(privateKeyInt);
    }

    std::string SHA(const CryptoPP::Integer& x, const CryptoPP::Integer& y) const {
        // Convert CryptoPP::Integer to byte arrays
        CryptoPP::SecByteBlock xBytes(x.MinEncodedSize()), yBytes(y.MinEncodedSize());
        x.Encode(xBytes, xBytes.size());
        y.Encode(yBytes, yBytes.size());

        // Hashing process
        CryptoPP::SHA256 sha256;
        sha256.Update(xBytes, xBytes.size());
        sha256.Update(yBytes, yBytes.size());

        // Convert hash to hex string
        std::string hash_str;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hash_str));
        sha256.TruncatedFinal(encoder);

        return "0x" + hash_str;
    }
};
