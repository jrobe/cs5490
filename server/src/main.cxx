#include <iostream>

//Other
#include "Log.h"
#include "AuthServeHandler.h"
#include "Utils.h"

//
#include <cassert>

//Thrift libraries
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
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

    //Sanity test
    byte* lol = Utils::generateRandom(32);
    lol[0] = 0x00;
    lol[1] = 0xff;
    std::string h = Utils::hex(lol,32);
    assert(h.size() == 64);
    int outLen = 0;
//    std::cout << "Hex: " << h << std::endl;
    byte* decout = Utils::fromHex((h.c_str()),h.size(),outLen);
    assert(outLen == 32);
    for(int i = 0; i < 32; i++)
    {
 //       std::cerr << i << std::endl;
  //      std::cerr << (int)lol[i] << " " << (int)decout[i] << std::endl;
        assert(lol[i] == decout[i]);
    }



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

    TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);

    server.serve();

    return 0;
}
