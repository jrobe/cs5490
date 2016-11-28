#include "AuthServeHandler.h"

#include <exception>

AuthServeHandler::AuthServeHandler()
: _db(Globals::DbPath)
{
    logInfo << "Initializing server";
    if(!_db.good())
    {
        logError << "Auth Handler initialization failed, database could not be opened; dying";
        throw std::runtime_error("Failed to open DB");
    }

}

void AuthServeHandler::createAccount(std::string& _return, const std::string& userName, const std::map<std::string, std::string> & keyValues) 
{
    logDebug << "Got a create account request";
    //TODO: Serialize the keyValues and encrypt; for now insert junk
    std::string serialized = "TEST";
    std::string errFromDb = _db.insertUser(userName,serialized);
    logDebug << std::string("Database returned error: ") + errFromDb;
    if(errFromDb.size() == 0)
    {
        _return = "YOURKEYHERE";
    }
    else
    {
        logError << std::string("Throwing error in createAccount: ") + errFromDb;
        error err;
        err.reason = errFromDb;
        throw  err;
    }
}

