#include <iostream>
#include <vector>
#include <stdexcept>
#include "../cryptopp/keccak.h"
#include "../cryptopp/sha.h"
#include "../cryptopp/hmac.h"
#include "../cryptopp/ripemd.h"
#include "../cryptopp/blake2.h"
#pragma once
class Hash {
public:
    Hash() = delete;

    static std::vector<uint8_t> hash(const std::vector<uint8_t>& input, const std::string& algorithm) {
        if (algorithm == "SHA-256") {
            return sha256(input);
        } else if (algorithm == "SHA3-256") {
            return sha3(input);
        } else if (algorithm == "HmacSHA512") {
            // Replace 'your_key_here' with your actual key
            std::vector<uint8_t> key = {'y', 'o', 'u', 'r', '_', 'k', 'e', 'y', '_', 'h', 'e', 'r', 'e'};
            return hmacSha512(key, input);
        } else if (algorithm == "RIPEMD160") {
            return ripemd160(input);
        } else if (algorithm == "Blake2b-256") {
            return blake2b256(input);
        } else {
            throw std::runtime_error("Unsupported hash algorithm: " + algorithm);
        }
    }

    static std::vector<uint8_t> sha3(const std::vector<uint8_t>& input) {
        CryptoPP::Keccak_256 keccak;
        keccak.Update(input.data(), input.size());
        std::vector<uint8_t> result(keccak.DigestSize());
        keccak.Final(result.data());
        return result;
    }

    static std::vector<uint8_t> sha256(const std::vector<uint8_t>& input) {
        CryptoPP::SHA256 sha256;
        sha256.Update(input.data(), input.size());
        std::vector<uint8_t> result(sha256.DigestSize());
        sha256.Final(result.data());
        return result;
    }

    static std::vector<uint8_t> hmacSha512(const std::vector<uint8_t>& key, const std::vector<uint8_t>& input) {
        CryptoPP::HMAC<CryptoPP::SHA512> hmac(key.data(), key.size());
        hmac.Update(input.data(), input.size());
        std::vector<uint8_t> result(hmac.DigestSize());
        hmac.Final(result.data());
        return result;
    }

    static std::vector<uint8_t> ripemd160(const std::vector<uint8_t>& input) {
        CryptoPP::RIPEMD160 ripemd160;
        ripemd160.Update(input.data(), input.size());
        std::vector<uint8_t> result(ripemd160.DigestSize());
        ripemd160.Final(result.data());
        return result;
    }

    static std::vector<uint8_t> blake2b256(const std::vector<uint8_t>& input) {
        CryptoPP::BLAKE2b blake2b;
        blake2b.Update(input.data(), input.size());
        std::vector<uint8_t> result(blake2b.DigestSize());
        blake2b.Final(result.data());
        return result;
    }
};

// int main() {
//     // Example usage:
//     std::vector<uint8_t> inputData = {'a', 'b', 'c'};
//     std::vector<uint8_t> sha256Result = Hash::hash(inputData, "SHA-256");

//     // Print the result
//     std::cout << "SHA-256 Result:";
//     for (auto byte : sha256Result) {
//         std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
//     }
//     std::cout << std::endl;

//     return 0;
// }
