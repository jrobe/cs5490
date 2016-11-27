#!/usr/bin/python
import sys
import glob


# To run this, you need to generate the Python thrift files
# thrift -gen py cs5490.thrift
# then move the gen-py folder to this folder 

sys.path.append('gen-py')
sys.path.append('/usr/lib/python2.7/site-packages/') #wasn't in my path?

from cs5490 import sanity

from thrift import Thrift
from thrift.transport import TSSLSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol


trans = TSSLSocket.TSSLSocket(host='127.0.0.1',port=9090,ca_certs="../ssl/server.crt",validate=False)
trans_buf = TTransport.TBufferedTransport(trans)
proto = TBinaryProtocol.TBinaryProtocol(trans_buf)
client = sanity.Client(proto)


trans.open()
print client.sanityCheck()
trans.close()
