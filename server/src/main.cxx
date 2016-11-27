#include <iostream>

//Other
#include "Log.h"
#include "AuthServeHandler.h"

//Thrift libraries
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSSLSocket.h>
#include <thrift/transport/TSSLServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using namespace  ::cs;

int main()
{

    //https://thrift.apache.org/lib/cpp
    boost::shared_ptr<TSSLSocketFactory> factory(new TSSLSocketFactory());
    //Client only
    factory->loadTrustedCertificates("../ssl/server.crt"); 

    factory->loadCertificate("../ssl/server.crt");
    factory->loadPrivateKey("../ssl/server.key");

    int port = 9090;
    boost::shared_ptr<AuthServeHandler> handler(new AuthServeHandler());
    boost::shared_ptr<TProcessor> processor(new AuthServeProcessor(handler));
    boost::shared_ptr<TServerTransport> serverTransport(new TSSLServerSocket(port,factory));
    boost::shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);

    server.serve();

    return 0;
}
