#include <iostream>
#include <string>
#include "Transaction.h"

class JoinTxn : public Transaction {
private:
    std::string validator;

public:
    JoinTxn(int transactionSize, int positionInBlock, long transactionFee,
            const std::string& type, const std::string& from, const std::string& to,
            const std::string& nonceOrValidationHash, const std::string& hash)
        : Transaction(transactionSize, positionInBlock, transactionFee, type, from, to, nonceOrValidationHash, hash),
          validator(from) {}

    // Getter

    /**
     * @return the validator the user is joining
     */
    std::string getValidator() const {
        return validator;
    }
};

// int main() {
//     // Example usage
//     JoinTxn joinTxn( /* provide constructor parameters here */ );

//     std::cout << "Validator: " << joinTxn.getValidator() << std::endl;

//     return 0;
// }
