#include <iostream>
#include <string>
#include "Transaction.h"

class DelegateTxn : public Transaction {
private:
    std::string validator;
    long amount;

public:
    DelegateTxn(int transactionSize, int positionInBlock, long transactionFee,
                const std::string& type, const std::string& from, const std::string& to,
                const std::string& nonceOrValidationHash, const std::string& hash, long amount)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          validator(to),
          amount(amount) {}

    // Getters

    /**
     * @return the validator the user is delegating to
     */
    std::string getValidator() const {
        return validator;
    }

    /**
     * @return the amount of PWR the user is delegating
     */
    long getAmount() const {
        return amount;
    }

    // Override the getValue method from the base class if necessary
    long getValue() const override {
        return amount;
    }
};

// int main() {
//     // Example usage
//     DelegateTxn delegateTxn( /* provide constructor parameters here */ );

//     std::cout << "Validator: " << delegateTxn.getValidator() << std::endl;
//     std::cout << "Amount: " << delegateTxn.getAmount() << std::endl;

//     return 0;
// }
