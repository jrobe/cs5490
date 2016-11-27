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

#include <tuple>

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

    private:
        AuthDB _db;

};

#endif
