#include <iostream>
#include <string>
#include "Transaction.h"

class WithdrawTxn : public Transaction {
private:
    std::string validator;
    long shares;

public:
    WithdrawTxn(int transactionSize, int positionInBlock, long transactionFee,
                const std::string& type, const std::string& from, const std::string& to,
                const std::string& nonceOrValidationHash, const std::string& hash, long shares)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          validator(to),
          shares(shares) {}

    // Getters

    /**
     * @return the validator the user is withdrawing from
     */
    const std::string& getValidator() const {
        return validator;
    }

    /**
     * @return the amount of PWR the user is withdrawing
     */
    long getShares() const {
        return shares;
    }
};

// int main() {
//     // Example usage
//     WithdrawTxn withdrawTxn( /* provide constructor parameters here */ );

//     std::cout << "Validator: " << withdrawTxn.getValidator() << std::endl;
//     std::cout << "Shares: " << withdrawTxn.getShares() << std::endl;

//     return 0;
// }
