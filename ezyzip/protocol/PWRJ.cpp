#include "PWRJ.h"
#include "../Validator/Validator.h"
static std::vector<Validator> getAllValidators() {
    try {
        std::string url = PWRJ::getRpcNodeUrl() + "/allValidators/";
        std::string response = performHttpRequest(url);

        json root = json::parse(response);
        if (root.find("validators") != root.end()) {
            const json& validators = root["validators"];
            std::vector<Validator> validatorsList;

            for (const auto& validatorObject : validators) {
                Validator validator(
                    "0x" + validatorObject["address"].get<std::string>(),
                    validatorObject["ip"].get<std::string>(),
                    validatorObject["badActor"].get<bool>(),
                    validatorObject["votingPower"].get<long>(),
                    validatorObject["totalShares"].get<long>(),
                    validatorObject["delegatorsCount"].get<int>(),
                    validatorObject["status"].get<std::string>()
                );
                validatorsList.push_back(validator);
            }

            return validatorsList;
        } else {
            throw std::runtime_error("Invalid JSON format: 'validators' not found.");
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return std::vector<Validator>();
    }
}
