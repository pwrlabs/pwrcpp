#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
#include <vector>
#include <string>
#include "../json.hpp"
#include "../Transaction/DelegateTxn.h"
#include "../Transaction/JoinTxn.h"
#include "../Transaction/Transaction.h"
#include "../Transaction/TransferTxn.h"
#include "../Transaction/VmDataTxn.h"
#include "../Transaction/WithdrawTxn.h"


class Block {
private:
    int transactionCount;
    int size;
    long number;
    long reward;
    long timestamp;
    std::string hash;
    std::string submitter;
    bool success;
    std::vector<Transaction*> transactions;

public:
    Block(const nlohmann::json& blockJson) {
        transactionCount = blockJson["transactionCount"].get<int>();
        size = blockJson["blockSize"].get<int>();
       
number = blockJson["blockNumber"].get<long long>(); // Use get<long long>() for asInt64()
reward = blockJson["blockReward"].get<long long>(); // Use get<long long>() for asInt64()
timestamp = blockJson["timestamp"].get<long long>(); // Use get<long long>() for asInt64()
hash = blockJson["blockHash"].get<std::string>(); // Use get<std::string>() for asString()
submitter = blockJson["blockSubmitter"].get<std::string>(); // Use get<std::string>() for asString()
success = blockJson["success"].get<bool>(); // Use get<bool>() for asBool()


        const nlohmann::json& txns = blockJson["transactions"];
        transactions.reserve(txns.size());

        for (const auto& txnObject : txns) {
            Transaction* txn = nullptr;

            std::string txnType = txnObject.contains("type") ? txnObject["type"].get<std::string>() : "Unknown";
           if (txnType == "Transfer") {
    txn = new TransferTxn(txnObject["size"], txnObject["positionInTheBlock"],
                           txnObject["fee"], txnType, txnObject["from"],
                           txnObject["to"], txnObject["nonceOrValidationHash"],
                           txnObject["hash"], txnObject["value"]);
} else if (txnType == "VM Data") {
    txn = new VmDataTxn(txnObject["size"], txnObject["positionInTheBlock"],
                        txnObject["fee"], txnType, txnObject["from"],
                        txnObject["to"], txnObject["nonceOrValidationHash"],
                        txnObject["hash"], txnObject["vmId"], txnObject["data"]);
} else if (txnType == "Delegate") {
    txn = new DelegateTxn(txnObject["size"], txnObject["positionInTheBlock"],
                          txnObject["fee"], txnType, txnObject["from"],
                          txnObject["to"], txnObject["nonceOrValidationHash"],
                          txnObject["hash"], txnObject["value"]);
} else if (txnType == "Withdraw") {
    txn = new WithdrawTxn(txnObject["size"], txnObject["positionInTheBlock"],
                           txnObject["fee"], txnType, txnObject["from"],
                           txnObject["to"], txnObject["nonceOrValidationHash"],
                           txnObject["hash"], txnObject["shares"]);
} else if (txnType == "Validator Join") {
    txn = new JoinTxn(txnObject["size"], txnObject["positionInTheBlock"],
                      txnObject["fee"], txnType, txnObject["from"],
                      txnObject["to"], txnObject["nonceOrValidationHash"],
                      txnObject["hash"]);
} else {
    txn = new Transaction(txnObject["size"], txnObject["positionInTheBlock"],
                           txnObject["fee"], txnType, txnObject["from"],
                           txnObject["to"], txnObject["nonceOrValidationHash"],
                           txnObject["hash"]);
}

            transactions.push_back(txn);
        }
    }

    // Destructor to free allocated memory
    ~Block() {
        for (auto& txn : transactions) {
            delete txn;
        }
    }

    // Getters

    /**
     * @return the number of transactions in the block
     */
    int getTransactionCount() const {
        return transactionCount;
    }

    /**
     * @return the byte size of the block
     */
    int getSize() const {
        return size;
    }

    /**
     * @return the number of the block
     */
    long getNumber() const {
        return number;
    }

    /**
     * @return the reward of the block
     */
    long getReward() const {
        return reward;
    }

    /**
     * @return the timestamp of the block
     */
    long getTimestamp() const {
        return timestamp;
    }

    /**
     * @return the hash of the block
     */
    const std::string& getHash() const {
        return hash;
    }

    /**
     * @return the address of the submitter of the block
     */
    const std::string& getSubmitter() const {
        return submitter;
    }

    /**
     * @return whether the block was processed without issues
     */
    bool isSuccess() const {
        return success;
    }

    /**
     * @return the transactions in the block
     */
    const std::vector<Transaction*>& getTransactions() const {
        return transactions;
    }
};
#endif // BLOCK_H
// int main() {
//     // Example usage:
//     Json::Value blockJson;
//     // Populate blockJson with your JSON data
//     Block block(blockJson);

//     std::cout << "Transaction Count: " << block.getTransactionCount() << std::endl;
//     std::cout << "Size: " << block.getSize() << std::endl;
//     std::cout << "Number: " << block.getNumber() << std::endl;
//     std::cout << "Reward: " << block.getReward() << std::endl;
//     std::cout << "Timestamp: " << block.getTimestamp() << std::endl;
//     std::cout << "Hash: " << block.getHash() << std::endl;
//     std::cout << "Submitter: " << block.getSubmitter() << std::endl;
//     std::cout << "Success: " << block.isSuccess() << std::endl;

//     const auto& transactions = block.getTransactions();
//     for (const auto& txn : transactions) {
//         // Handle transactions as needed
//     }

//     return 0;
// }
