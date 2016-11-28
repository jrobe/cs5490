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
    byte* key = Utils::generateRandom(32);
    std::string data = ss.str();
    int outLen = 0;

    byte* encrypted = Utils::encrypt((byte*)data.c_str(),data.size()+1,key,32,outLen);

    std::string encryptedStr = std::string(Utils::hex(encrypted,outLen));
    //Store everything hex to be easy

#ifdef DEBUGMODE
    logDebug << "Performing decryption sanity";
    int decLen = 0;
    byte* decrypted = Utils::decrypt(encrypted,outLen,key,32,decLen);
    std::string decStr = std::string((char*)decrypted);
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

void AuthServeHandler::retrieveWithKey(std::map<std::string, std::string> & _return, const std::string& userName, const std::string& key) 
{
    //Key should be in hex

    int keyLen = 0;
    byte* rawKey = Utils::fromHex(key.c_str(),key.size(),keyLen);
    assert(keyLen == 32);

    logDebug << "Retrieving from the database with a key";
    int len = 0 ;
    char* encryptedData = _db.getEncryptedUserData(userName,len); //hex
    int outLen = 0;;
    byte* encryptedRaw = Utils::fromHex(encryptedData,len,outLen);
    int decLen = 0;
    byte* decrypted = Utils::decrypt((byte*)encryptedRaw,outLen,rawKey,32,decLen); 

/*
    std::string decryptedStr(decrypted);
    std::istringstream ssi(decryptedStr);
    std::string token;
    while(std::getline(ssi,token,";"))
    {
        logDebug << token;
    }
    */
    //_return = std::string(encryptedData,len);

}


void AuthServeHandler::requestPermission(PermissionRequest& _return, const std::string& userName) {
    // Your implementation goes here
    printf("requestPermission\n");
}

void AuthServeHandler::checkForPermissionRequests(std::vector<PermissionRequest> & _return) {
    // Your implementation goes here
    printf("checkForPermissionRequests\n");
}

void AuthServeHandler::decideRequest(const PermissionRequest& request, const bool decision, const std::string& reason, const std::string& key) {
    // Your implementation goes here
    printf("decideRequest\n");
}

void AuthServeHandler::checkForPermissionGranted(PermissionRequest& _return, const int64_t requestID) {
    // Your implementation goes here
    printf("checkForPermissionGranted\n");
}
