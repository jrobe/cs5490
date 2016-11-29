#!/usr/bin/python
import sys
import glob
import binascii
import random
import string


# To run this, you need to generate the Python thrift files
# thrift -gen py cs5490.thrift
# then move the gen-py folder to this folder 

sys.path.append('gen-py')
sys.path.append('/usr/lib/python2.7/site-packages/') #wasn't in my path?

from cs5490 import AuthServe

from thrift import Thrift
from thrift.transport import TSSLSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol


trans = TSSLSocket.TSSLSocket(host='127.0.0.1',port=9090,ca_certs="../ssl/server.crt",validate=False)
trans_buf = TTransport.TBufferedTransport(trans)
proto = TBinaryProtocol.TBinaryProtocol(trans_buf)
client = AuthServe.Client(proto)



def run(good):
    testUserName = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))
    trans.open()
    hexKey = client.createAccount(testUserName, {'email':'testing@test.com', 'cc' : '1234-5555-5555-5555', 'dob' : '1969-12-28'})
    print "Key: " + hexKey


#Demo
    request = client.requestPermission(testUserName)
    print 'Request ID: ' + str(request.requestID)
    print request

#Portal
    portalRequests = client.checkForPermissionRequests()
    for pr in portalRequests:
        pr.requestID = request.requestID
        client.decideRequest(pr,good,"Access Granted",hexKey)

#demo
    result = client.checkForPermissionGranted(request.requestID)


    if result.granted:
        for key in result.results.keys():
            print key + " => " + result.results[key]
    else:
        print 'Permission denied!'

    trans.close()


run(True)
run(False)
