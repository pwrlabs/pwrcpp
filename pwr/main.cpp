#include <iostream>
#include "Utils/Response.h"
#include "protocol/PWRJ.h"
#include "PWRWallet.h"
#include <curl/curl.h>
int main() {
    
    PWRJ::setRpcNodeUrl("https://pwrrpc.pwrlabs.io/");
    PWRWallet wallet("48157030754737918552913355043337580418007638602253380155554472945119041505152");

    std::wcout << L"Address: " << std::wstring(wallet.getAddress().begin(), wallet.getAddress().end()) << std::endl;
   std::string address = wallet.getAddress();
    Response response(address);
    
    std::cout << "Success: " << response.isSuccess() << std::endl;
    std::cout << "Txn Hash: " << response.getTxnHash() << std::endl;
    std::cout << "Error: " << response.getError() << std::endl;
    
    return 0;
}
