#include "AuthServeHandler.h"


AuthServeHandler::AuthServeHandler()
{
    logInfo << "Initializing server";
}

void AuthServeHandler::createAccount(std::string& _return, const std::string& userName, const std::map<std::string, std::string> & keyValues) 
{
    logDebug << "Called create account";
}

