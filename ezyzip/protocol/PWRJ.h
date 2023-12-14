#ifndef PWRJ_H
#define PWRJ_H

#include <iostream>
#include <string>
#include <vector>
#include "../json.hpp"
#include <curl/curl.h> 
#include "../Utils/Hash.h"
#include "../Validator/Validator.h"
#include "../PWRWallet.h"
#include "../Block/Block.h"

using json = nlohmann::json;
using namespace std;

class PWRJ {
private:
    static CURL* curl;
    static std::string rpcNodeUrl;
    static long feePerByte;

public:
    static void setRpcNodeUrl(const std::string& url) {
        rpcNodeUrl = url;
    }

    static std::string getRpcNodeUrl() {
        return rpcNodeUrl;
    }

    static long getFeePerByte() {
        return feePerByte;
    }

    static int getNonceOfAddress(const std::string& address) {
        try {
            std::string url = rpcNodeUrl + "/nonceOfUser/?userAddress=" + address;
            std::string response = performHttpRequest(url);

            json object = json::parse(response);
            return object["nonce"];
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 0;
        }
    }

    static long getBalanceOfAddress(const std::string& address) {
        try {
            std::string url = rpcNodeUrl + "/balanceOf/?userAddress=" + address;
            std::string response = performHttpRequest(url);

            json object = json::parse(response);
            return object["balance"];
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 0;
        }
    }

    static long getBlocksCount() {
        try {
            std::string url = rpcNodeUrl + "/blocksCount/";
            std::string response = performHttpRequest(url);

            json root = json::parse(response);
            if (root.find("blocksCount") != root.end()) {
                return root["blocksCount"].get<long>();
            } else {
                throw std::runtime_error("Invalid JSON format: 'blocksCount' not found.");
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            return 0;
        }
    }

    static long getLatestBlockNumber() {
        return getBlocksCount() - 1;
    }

    // Define Block class or use appropriate data structure

    static Block getBlockByNumber(long blockNumber) {
    
        std::string url = rpcNodeUrl + "/block/?blockNumber=" + std::to_string(blockNumber);
        std::string response = performHttpRequest(url);

        json root;
        json reader;

        if (reader.parse(response)) {
            if (root.contains("block")) {
                // Assuming Block class has a constructor that takes a json::value_type
                return Block(root["block"]);
            } else {
                throw std::runtime_error("Invalid JSON format: 'block' not found.");
            }
        } else {
            throw std::runtime_error("Failed to parse JSON response.");
        }
  
}


    // ... Other methods ...

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    static std::string performHttpRequest(const std::string& url) {
        std::string response;
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                throw std::runtime_error("Failed to perform HTTP request: " + std::string(curl_easy_strerror(res)));
            }
        }
        return response;
    }





static int  parseValidatorsCount(const std::string& url) {
    try {
        std::string response = performHttpRequest(url);
        json root = json::parse(response);

        if (root.find("validatorsCount") != root.end()) {
            return root["validatorsCount"].get<int>();
        } else {
            throw std::runtime_error("Invalid JSON format: 'validatorsCount' not found.");
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 0;
    }
}

static int getTotalValidatorsCount() {
    
    std::string url = rpcNodeUrl + "/totalValidatorsCount/";
    return parseValidatorsCount(url);
}

static int getStandbyValidatorsCount() {
    std::string url = rpcNodeUrl + "/standbyValidatorsCount/";
    return parseValidatorsCount(url);
}

static int getActiveValidatorsCount() {
    std::string url = rpcNodeUrl + "/activeValidatorsCount/";
    return parseValidatorsCount(url);
}


    static std::vector<Validator> getAllValidators(); 

static long getDelegatedPWR(const std::string& delegatorAddress, const std::string& validatorAddress) {
    try {
        std::string url = rpcNodeUrl + "/validator/delegator/delegatedPWROfAddress/?userAddress=" + delegatorAddress + "&validatorAddress=" + validatorAddress;
        std::string response = performHttpRequest(url);

        json root = json::parse(response);
        if (root.find("delegatedPWR") != root.end()) {
            return root["delegatedPWR"];
        } else if (root.find("message") != root.end()) {
            throw std::runtime_error("Failed with HTTP error 400 and message: " + root["message"].get<std::string>());
        } else {
            throw std::runtime_error("Failed with HTTP error code: " + std::to_string(response.empty() ? 0 : -1));
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 0;
    }
}
json getShareValue(const std::vector<std::string>& validators, long blockNumber) {
    try {
        std::string url = rpcNodeUrl + "/getShareValue/";

        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize libcurl");
        }

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        json jsonPayload;
        jsonPayload["validators"] = validators;
        jsonPayload["blockNumber"] = blockNumber;

        std::string jsonPayloadStr = jsonPayload.dump();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayloadStr.c_str());

        std::string responseString;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            json object = json::parse(responseString);
            return object["shareValues"];
        } else {
            throw std::runtime_error("Failed with libcurl error: " + std::to_string(res));
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return json();
    }
}
std::string getOwnerOfVm(long vmId) {
    try {
        // Construct the URL for the request
        std::string url = rpcNodeUrl + "/ownerOfVmId/?vmId=" + std::to_string(vmId);

        // Initialize libcurl
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize libcurl");
        }

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to handle the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the HTTP GET request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to perform HTTP request: " + std::string(curl_easy_strerror(res)));
        }

        // Clean up
        curl_easy_cleanup(curl);

        // Parse the JSON response and return the owner's address
        json object = json::parse(response);
        return object["owner"];
    } catch (const std::exception& e) {
        // Handle exceptions and log the error
        std::cerr << e.what() << std::endl;
        // Return a default value in case of an error
        return "0x0000000000000000000000000000000000000000";
    }
}

void updateFeePerByte() {
    try {
        // Construct the URL for the request
        std::string url = rpcNodeUrl + "/feePerByte/";

        // Initialize libcurl
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize libcurl");
        }

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to handle the response
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the HTTP GET request
        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to perform HTTP request: " + std::string(curl_easy_strerror(res)));
        }

        // Clean up
        curl_easy_cleanup(curl);

        // Parse the JSON response and update the feePerByte variable
        json object = json::parse(response);
        feePerByte = object["feePerByte"];
    } catch (const std::exception& e) {
        // Handle exceptions and log the error
        std::cerr << e.what() << std::endl;
        // You may want to rethrow the exception here or handle it as needed
    }
}


// Assuming the Hash class and Response class are defined elsewhere
static Response broadcastTxn(const std::vector<uint8_t>& txn) {
     
        try {
            // Initialize libcurl if not done already
            if (!curl) {
                curl_global_init(CURL_GLOBAL_DEFAULT);
                curl = curl_easy_init();
            }

            // Check if libcurl initialization was successful
            if (!curl) {
                throw std::runtime_error("Failed to initialize libcurl");
            }

            // Construct the URL for the request
            std::string url = rpcNodeUrl + "/broadcast/";

            // Prepare the JSON payload with the transaction data
            json jsonPayload;
            jsonPayload["txn"] = Hash::to_hex_string(Hash::sha3(txn));
            std::string jsonStr = jsonPayload.dump();

            // Set up libcurl options
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, "Content-Type: application/json");

            // Perform the libcurl request
            CURLcode res = curl_easy_perform(curl);

            // Check the result of the libcurl request
            if (res != CURLE_OK) {
                throw std::runtime_error("Failed to perform HTTP request with libcurl: " + std::string(curl_easy_strerror(res)));
            }

            // Assuming you have a function to process the response and create a Response object
            return processCurlResponse(curl,txn);

        } catch (const std::exception& e) {
            return Response(false, nullptr, e.what());
      
      
      
        }
    }
    private:
    // Helper function to process libcurl response and create a Response object
    static Response processCurlResponse(CURL* curl, const std::vector<uint8_t>&  txn) {
    long responseCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

    if (responseCode == 200) {
        // If successful, return a Response object with success and the hash of the transaction
        std::string txnHash = "0x" + Hash::to_hex_string(Hash::sha3(txn));
        return Response(true, txnHash, "");
    } else {
        // Handle other HTTP errors by throwing a runtime error with the HTTP status code
        throw std::runtime_error("Failed with HTTP error code: " + std::to_string(responseCode));
    }
}


};
#endif // PWRJ_H
// // Initialize static members
// httplib::Client PWRJ::client = httplib::Client("localhost", 80); // Update with actual host and port
// std::string PWRJ::rpcNodeUrl;
// long PWRJ::feePerByte;

// int main() {
//     PWRJ::setRpcNodeUrl("http://example.com/rpc"); // Replace with the actual RPC URL

//     // Example usage
//     auto nonce = PWRJ::getNonceOfAddress("0x123456789abcdef");
//     std::cout << "Nonce: " << nonce << std::endl;

//     auto balance = PWRJ::getBalanceOfAddress("0x123456789abcdef");
//     std::cout << "Balance: " << balance << std::endl;

//     auto blocksCount = PWRJ::getBlocksCount();
//     std::cout << "Blocks Count: " << blocksCount << std::endl;

//     PWRJ::updateFeePerByte();
//     std::cout << "Fee Per Byte: " << PWRJ::getFeePerByte() << std::endl;

//     return 0;
// }

