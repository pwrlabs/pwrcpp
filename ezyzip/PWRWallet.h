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
     CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP> params;
    // Initialize the private key
    privateKey.Initialize(rng, params);
    privateKey.SetPrivateExponent(privateKeyInt);
}


     std::string SHA(const CryptoPP::Integer& x, const CryptoPP::Integer& y) const {
    // Convert CryptoPP::Integer to byte arrays
    CryptoPP::SecByteBlock xBytes(x.MinEncodedSize()), yBytes(y.MinEncodedSize());
    x.Encode(xBytes, xBytes.size());
    y.Encode(yBytes, yBytes.size());

    // Concatenate byte arrays
    CryptoPP::SecByteBlock concatenatedBytes(xBytes.size() + yBytes.size());
    std::memcpy(concatenatedBytes, xBytes, xBytes.size());
    std::memcpy(concatenatedBytes + xBytes.size(), yBytes, yBytes.size());

    // Hashing process
    CryptoPP::SHA256 sha256;
    sha256.Update(concatenatedBytes, concatenatedBytes.size());

    // Convert hash to hex string
    std::string hash_str;
    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hash_str), false); // 'false' for uppercase hex
    sha256.TruncatedFinal(concatenatedBytes, sha256.DigestSize());
    encoder.MessageEnd();

    return "0x" + hash_str;
}
};
