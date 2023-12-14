#ifndef VALIDATOR_H
#define VALIDATOR_H

                                          

#include <iostream>
#include <string>
#include <vector>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/base_uri.h>
#include <stdexcept>
#include "../Delegator/Delegator.h"
#include "../Utils/Hash.h"


class PWRJ;


class Validator {
private:
    std::string address;
    std::string ip;
    bool badActor;
    long votingPower;
    long shares;
    int delegatorsCount;
    std::string status;
    

public:
    Validator(const std::string& address, const std::string& ip, bool badActor, long votingPower, long shares, int delegatorsCount, const std::string& status)
        : address(address), ip(ip), badActor(badActor), votingPower(votingPower), shares(shares), delegatorsCount(delegatorsCount), status(status) {}

    std::string getAddress() const {
        return address;
    }

    std::string getIp() const {
        return ip;
    }

    bool isBadActor() const {
        return badActor;
    }

    long getVotingPower() const {
        return votingPower;
    }

    long getShares() const {
        return shares;
    }

    int getDelegatorsCount() const {
        return delegatorsCount;
    }

    std::string getStatus() const {
        return status;
    }

   std::vector<Delegator> getDelegators(const PWRJ& pwrj) const; 

};
#endif // VALIDATOR_H

// int main() {
//     Validator validator("validatorAddress", "127.0.0.1", false, 1000, 500, 10, "active");
    
//     // Uncomment and use other validator functions as needed

//     return 0;
// }
