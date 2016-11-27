#include "AuthServeHandler.h"


AuthServeHandler::AuthServeHandler()
: _db(Globals::DbPath)
{
    logInfo << "Initializing server";

}

void AuthServeHandler::createAccount(std::string& _return, const std::string& userName, const std::map<std::string, std::string> & keyValues) 
{
    logDebug << "Called create account";
}

