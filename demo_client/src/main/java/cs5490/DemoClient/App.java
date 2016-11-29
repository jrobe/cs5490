package cs5490.DemoClient;

import java.util.HashMap;
import javax.swing.JFrame;

import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;

import cs.*;

public class App extends JFrame {
	public static void main(String[] args) {
		try {
			TTransport transport;
			TSSLTransportParameters params = new TSSLTransportParameters();
			params.setTrustStore("../ssl/truststore.jks", "cs5490");
			transport = TSSLTransportFactory.getClientSocket("localhost", 9090, 0, params);
			TProtocol protocol = new  TBinaryProtocol(transport);
			AuthServe.Client client = new AuthServe.Client(protocol);
			
			// Do stuff...
			new App().setVisible(true);
			client.createAccount("testing", new HashMap<String, String>());

			transport.close();
		} catch (Exception e) {
			System.out.println(e);
		}
	}

	public App() {
		setTitle("Demo Client");
        setSize(300, 200);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
	}
}
