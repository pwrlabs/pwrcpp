#include <iostream>
#include <string>
#include "Transaction.h"

class TransferTxn : public Transaction {
private:
    long value;

public:
    TransferTxn(int transactionSize, int positionInBlock, long transactionFee,
                const std::string& type, const std::string& from, const std::string& to,
                const std::string& nonceOrValidationHash, const std::string& hash, long value)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          value(value) {}

    // Getter

    /**
     * @return the value of the transaction
     */
    long getValue() const override {
        return value;
    }
};

// int main() {
//     // Example usage
//     TransferTxn transferTxn( /* provide constructor parameters here */ );

//     std::cout << "Value: " << transferTxn.getValue() << std::endl;

//     return 0;
// }
