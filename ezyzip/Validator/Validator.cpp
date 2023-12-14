 
#include "Validator.h"
#include "../protocol/PWRJ.h
 
 
 std::vector<Delegator> getDelegators() const {
    try {
        // web::http::client::http_client client(PWRJ::getRpcNodeUrl() + U("/validator/delegatorsOfValidator/?validatorAddress=") + web::uri::encode_data_string(address));
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