#include <string>
#pragma once
class Delegator {
private:
    std::string address;
    std::string validatorAddress;
    long shares;
    long delegatedPWR;

public:
    Delegator(const std::string& address, const std::string& validatorAddress, long shares, long delegatedPWR)
        : address(address), validatorAddress(validatorAddress), shares(shares), delegatedPWR(delegatedPWR) {}

    // Getters

    /**
     * @return the address of the delegator
     */
    const std::string& getAddress() const {
        return address;
    }

    /**
     * @return the address of the validator
     */
    const std::string& getValidatorAddress() const {
        return validatorAddress;
    }

    /**
     * @return the shares of the delegator
     */
    long getShares() const {
        return shares;
    }

    /**
     * @return the delegated PWR of the delegator
     */
    long getDelegatedPWR() const {
        return delegatedPWR;
    }
};

// int main() {
//     // Example usage:
//     Delegator delegator("delegator_address", "validator_address", 1000, 500);

//     // Accessing member variables using getters
//     std::cout << "Delegator Address: " << delegator.getAddress() << std::endl;
//     std::cout << "Validator Address: " << delegator.getValidatorAddress() << std::endl;
//     std::cout << "Shares: " << delegator.getShares() << std::endl;
//     std::cout << "Delegated PWR: " << delegator.getDelegatedPWR() << std::endl;

//     return 0;
// }
