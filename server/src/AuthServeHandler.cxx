#include "AuthServeHandler.h"

#include <exception>
#include <sstream>

#include "Utils.h"

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
    std::string serialized = "";

    std::stringstream ss;
    for(auto it = keyValues.begin(); it != keyValues.end(); it++)
    {
        //Could do JSON; lazy
        ss << it->first << ":" << it->second << ";";
    }
    char* key = Utils::generateRandom(32);
    std::string data = ss.str();
    int outLen = 0;

    char* encrypted = Utils::encrypt((char*)data.c_str(),data.size()+1,key,32,outLen);

    std::string encryptedStr = std::string(Utils::hex(encrypted,outLen));
    //Store everything hex to be easy

#ifdef DEBUGMODE
    logDebug << "Performing decryption sanity";
    int decLen = 0;
    char* decrypted = Utils::decrypt((char*)encrypted,outLen,key,32,decLen);
    std::string decStr = std::string(decrypted);
    delete[] decrypted;
    logDebug << std::string("Decrypt sanity: ") + decStr;
#endif

    std::string errFromDb = _db.insertUser(userName,encryptedStr);
    logDebug << std::string("Database returned error: ") + errFromDb;
    if(errFromDb.size() == 0)
    {
        _return = Utils::hex(key,32);
    }
    else
    {
        logError << std::string("Throwing error in createAccount: ") + errFromDb;
        error err;
        err.reason = errFromDb;
        delete[] key;
        delete[] encrypted;
        throw  err;
    }
    //TODO: Cleanup
    delete[] encrypted;
    delete[] key;
}

