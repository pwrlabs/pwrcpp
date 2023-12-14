#include <iostream>
#include <string>
#pragma once
class Response {
private:
    bool success;
    std::string txnHash;
    std::string error;

public:
    Response(bool success, std::string txnHash, std::string error)
        : success(success), txnHash(std::move(txnHash)), error(std::move(error)) {}

    /**
     * @return true if the operation was successful, false otherwise
     */
    bool isSuccess() const {
        return success;
    }

    /**
     * @return transaction hash if the operation was successful
     */
    const std::string& getTxnHash() const {
        return txnHash;
    }

    /**
     * @return error message if the operation was not successful
     */
    const std::string& getError() const {
        return error;
    }
};

// int main() {
//     // Example usage:
//     Response response(true, "0x123456789abcdef", "");
    
//     // Check if the operation was successful
//     if (response.isSuccess()) {
//         std::cout << "Transaction Hash: " << response.getTxnHash() << std::endl;
//     } else {
//         std::cerr << "Error: " << response.getError() << std::endl;
//     }

//     return 0;
// }
