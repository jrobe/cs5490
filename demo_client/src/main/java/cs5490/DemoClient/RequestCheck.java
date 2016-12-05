package cs5490.DemoClient;

import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;

import cs.AuthServe;
import cs.PermissionRequest;

interface RequestCheckCallback {
    void requestApproved(PermissionRequest pr); // would be in any signature
    void requestFailed(String reason); // would be in any signature
}

public class RequestCheck implements Runnable {
    
	volatile boolean shutdown = false;
	RequestCheckCallback c; 
    long requestId;
	
	RequestCheck(RequestCheckCallback c, long requestId) {
		  this.c = c;
		  this.requestId = requestId;
	}
	
	public void run() {
		while(!shutdown)
		{
			try
			{
				TTransport transport;
				TSSLTransportParameters params = new TSSLTransportParameters();
				params.setTrustStore("../ssl/truststore.jks", "cs5490");
				transport = TSSLTransportFactory.getClientSocket("localhost", 9090, 0, params);
				TProtocol protocol = new  TBinaryProtocol(transport);
				AuthServe.Client client = new AuthServe.Client(protocol);
				PermissionRequest pr = client.checkForPermissionGranted(requestId);
				transport.close();

				if(!pr.isSetGranted())
				{
					Thread.sleep(500);
				}
				else if(pr.granted)
				{
					this.c.requestApproved(pr);
					break;
				}
				else
				{
					this.c.requestFailed("Login Request Denied");
					break;
				}
				
			}
			catch (Exception e)
			{
				e.printStackTrace();
			    this.c.requestFailed(e.getMessage());
			}
		}
	}
	
}