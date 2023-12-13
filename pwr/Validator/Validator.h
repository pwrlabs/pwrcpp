
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
#include "../protocol/PWRJ.h"

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

   std::vector<Delegator> getDelegators() const {
    try {
        web::http::client::http_client client(PWRJ::getRpcNodeUrl() + U("/validator/delegatorsOfValidator/?validatorAddress=") + web::uri::encode_data_string(address));
        web::http::http_request request(web::http::methods::GET);
        web::http::http_response response = client.request(request).get();

        if (response.status_code() == web::http::status_codes::OK) {
            web::json::value jsonResponse = response.extract_json().get();
            web::json::object delegators = jsonResponse[U("delegators")].as_object();
            std::vector<Delegator> delegatorsList;

            for (const auto& pair : delegators) {
                const web::json::value& value = pair.second;
                if (value.is_integer()) {
                    const std::string delegatorAddress = utility::conversions::to_utf8string(pair.first);
                    long shares = value.as_integer();
                    long delegatedPWR = (shares * votingPower) / shares;
                   Delegator d("0x" + utility::conversions::to_utf8string(delegatorAddress), address, shares, delegatedPWR);

                    delegatorsList.push_back(d);
                } else {
                    throw std::runtime_error("Invalid format for delegators.");
                }
            }

            return delegatorsList;
        } else if (response.status_code() == web::http::status_codes::BadRequest) {
            web::json::value jsonResponse = response.extract_json().get();
            throw std::runtime_error("Failed with HTTP error 400 and message: " + jsonResponse[U("message")].serialize());

        } else {
            throw std::runtime_error("Failed with HTTP error code: " + std::to_string(response.status_code()));
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return std::vector<Delegator>();
    }
}

};
#endif // VALIDATOR_H

// int main() {
//     Validator validator("validatorAddress", "127.0.0.1", false, 1000, 500, 10, "active");
    
//     // Uncomment and use other validator functions as needed

//     return 0;
// }
