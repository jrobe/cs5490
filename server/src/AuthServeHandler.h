#ifndef __AuthServeHandler_h__
#define __AuthServeHandler_h__

#include "../generated/AuthServe.h"
#include "../generated/cs5490_types.h"
#include "../generated/cs5490_constants.h"

//Other
#include "AuthDB.h"
#include "Log.h"

//Thrift libraries
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSSLSocket.h>
#include <thrift/transport/TSSLServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <map>
#include <vector>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace  ::cs;

class AuthServeHandler : virtual public cs::AuthServeIf {

    public:
        //obj man
        AuthServeHandler();

        //Thrift Functions
        void createAccount(std::string& _return, const std::string& userName, const std::map<std::string, std::string> & keyValues);
        void retrieveWithKey(std::map<std::string, std::string> & _return, const std::string& userName, const std::string& key);
        void requestPermission(PermissionRequest& _return, const std::string& userName);
        void checkForPermissionRequests(std::vector<PermissionRequest> & _return);
        void decideRequest(const PermissionRequest& request, const bool decision, const std::string& reason, const std::string& key);
        void checkForPermissionGranted(PermissionRequest& _return, const int64_t requestID);

    private:
        AuthDB _db;
        std::map<std::string,std::string> _retrieveValues(const std::string& key,const std::string& userName);
        long long _generateId(PermissionRequest&);
        std::map<long long,PermissionRequest> _activeRequests;
        std::vector<long long> _pendingIds;
        std::vector<long long> _pendingResponses;

};

#endif
