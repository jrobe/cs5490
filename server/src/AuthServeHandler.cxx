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
        if(it->second.size() != 0 && it->first.size() != 0)
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

std::map<std::string,std::string> AuthServeHandler::_retrieveValues(const std::string& key,const std::string& userName)
{
    int keyLen = 0;
    byte* rawKey = Utils::fromHex(key.c_str(),key.size(),keyLen);
    if(keyLen != 32)
    {
        error err;
        err.reason = "Invalid key, should be 32 bytes";
        throw err;
    }

    int len = 0 ;
    char* encryptedData = _db.getEncryptedUserData(userName,len); //hex

    int outLen = 0;;
    byte* encryptedRaw = Utils::fromHex(encryptedData,len,outLen);

    int decLen = 0;
    byte* decrypted = Utils::decrypt((byte*)encryptedRaw,outLen,rawKey,32,decLen); 

    if(decLen == 0)
    {
        error err;
        err.reason = "Invalid decryption; corrupt key";
        throw err;
    }

    std::string decryptedStr((char*)decrypted,decLen); //Assume we stored it correctly
    std::istringstream ssi(decryptedStr);
    std::string token;
    std::map<std::string,std::string> _out;
    while(std::getline(ssi,token,';'))
    {
        logDebug << token;
        std::string innerToken;
        std::istringstream innerSsi(token);
        std::vector<std::string> tmpVec;
        while(std::getline(innerSsi,innerToken,':'))
        {
            tmpVec.push_back(innerToken);
        }
        if(tmpVec.size() != 2)
        {
            error err;
            err.reason = "Corrupt data in the database (bad decrypt?)";
            throw err;
        }
        _out[tmpVec[0]] = tmpVec[1];
    }
    
    return std::move(_out);

    
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

/*
struct PermissionRequest
{
    1: optional string user;
    2: optional i64 requestID;
    3: optional bool granted;
    4: optional string reason;
    5: optional map<string,string> results;
}
*/

long long AuthServeHandler::_generateId(PermissionRequest& perm)
{
    long long candidate = rand();
    while(_activeRequests.find(candidate) != _activeRequests.end())
        candidate = rand();

    perm.requestID = candidate;
    _activeRequests[candidate] = perm; //copy

    return candidate;
}

void AuthServeHandler::requestPermission(PermissionRequest& _return, const std::string& userName) {
    //The Demo app will request permission here;
    logDebug << "requestPermission";
    // just generate the permissionRequest
    if(!_db.checkIfUserExists(userName))
    {
        logError << "Requesting permission for a user that doesn't exist!";
        error err;
        err.reason = "The user doesn't exist to request permission for";
        throw  err;
    }

    
    _return.user =  userName;
    //Not strictly required
    _return.granted = false;
    _return.reason = "test";
    long long id = _generateId(_return); //do this last always; since we pass by reference, it will not copy anything else; we will be dumping this on the floor when we are done.
    logDebug << std::string("Requesting permission; assigned ID: ") + Utils::fromNumber(id);
    _pendingIds.push_back(id);
    _return.requestID = id;

}

void AuthServeHandler::checkForPermissionRequests(std::vector<PermissionRequest> & _return) {
    //logDebug << "Checking for permission requests";
    // This is the fake push ( really poll) that the portal does 
    for(long long id : _pendingIds)
    {
        //Ensure that these don't go out of sync somehow.
        _return.push_back(_activeRequests.at(id));
    }
    _pendingIds.clear();
}

void AuthServeHandler::decideRequest(const PermissionRequest& request, const bool decision, const std::string& reason, const std::string& key) {
    // This is what the portal sends to the server to let it know it's decision about the request
    logDebug << std::string("decideRequest for ID: ") + Utils::fromNumber(request.requestID);
    logDebug << std::string("Decide reason: ") + reason;
    logDebug << std::string("Decide key: ") + key;

    if(_activeRequests.find(request.requestID) == _activeRequests.end())
    {
        logError << "That request doesn't exist...";
        error err;
        err.reason = "That request doesn't exist...";
        throw  err;
    }

    logDebug << std::string("Deciding for request: ") + Utils::fromNumber(request.requestID);
    PermissionRequest& req = _activeRequests.at(request.requestID);
    if(decision)
    {
        //We have decided yes; decrypt with the key at this point, and store in the request.
        if(key.size() == 0)
        {
            error err;
            err.reason = "You decided yes, but failed to provide a key...";
            throw err;
        }

        req.results = _retrieveValues(key,req.user);

    }

    req.granted = decision;
    req.reason = reason;
    _pendingResponses.push_back(request.requestID);
}

void AuthServeHandler::checkForPermissionGranted(PermissionRequest& _return, const int64_t requestID) {
    // The debug app will check to see if it got permission here, the results will be returned in the _return if it is.
    //logDebug << "checkForPermissionGranted";
    //if(_pendingResponses.find(requestID) != _pendingResponses.end())
    if(std::find(_pendingResponses.begin(),_pendingResponses.end(),requestID) != _pendingResponses.end())
    {
        _return = _activeRequests[requestID];
        _activeRequests.erase(requestID);
        auto found = std::find(_pendingResponses.begin(),_pendingResponses.end(),requestID);
        _pendingResponses.erase(found);
    }
}
