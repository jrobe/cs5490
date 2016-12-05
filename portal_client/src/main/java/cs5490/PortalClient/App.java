package cs5490.PortalClient;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import layout.*;
import java.io.*;
import java.nio.*;
import java.nio.channels.*;

import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.TException;

import cs.*;

public class App {
	public static String keyFilename = "key.txt";

	public static void main(String[] args) {
		try {
			TTransport transport;
			TSSLTransportParameters params = new TSSLTransportParameters();
			params.setTrustStore("../ssl/truststore.jks", "cs5490");
			transport = TSSLTransportFactory.getClientSocket("localhost", 9090, 0, params);
			TProtocol protocol = new  TBinaryProtocol(transport);
			AuthServe.Client client = new AuthServe.Client(protocol);
			
			// Do stuff...
			if (new File(keyFilename).exists()) {
				checkForUpdates(client);
				transport.close();
				System.exit(0);
			} else {
				RegistrationForm app = new RegistrationForm(client);
				app.setVisible(true);
				app.addWindowListener(new WindowAdapter() {
					@Override
					public void windowClosing(WindowEvent windowEvent) {
						if (new File(keyFilename).exists()) {
							System.out.println("File does exist");
							checkForUpdates(client);
							transport.close();
							System.exit(0);
						} else {
							System.out.println("File doesn't exist");
							transport.close();
							System.exit(0);
						}
					}
				});
			}
		} catch (Exception e) {
			System.out.println(e);
		}
	}

	private static void checkForUpdates(AuthServe.Client client) {
		System.out.println("Checking for requests...");

		try {
			String key = readString(keyFilename);
			System.out.println("key: " + key);

			if (key != null) {
				while (true) {
					// Check for requests periodically
					java.util.List<PermissionRequest> requests = client.checkForPermissionRequests();
					if (requests.size() > 0) {
						System.out.println("There are " + requests.size() + " waiting requests.");
						for (PermissionRequest request : requests) {
							System.out.println(request.requestID);
							int dialogResult = JOptionPane.showConfirmDialog (null, "An application is requesting to login. Approve login?", "Login Request", JOptionPane.YES_NO_OPTION);
							client.decideRequest(request, (dialogResult == JOptionPane.YES_OPTION), "", key);
						}
					} else {
						System.out.println("No requests. Trying again soon.");
						Thread.sleep(250);
					}
				}
			} else {
				JOptionPane.showMessageDialog(null, "Error reading key file.", "Error", JOptionPane.ERROR_MESSAGE);
			}

			
		} catch (error e) {
			JOptionPane.showMessageDialog(null, e.getReason(), "Error", JOptionPane.ERROR_MESSAGE);
		} catch (Exception e) {
			JOptionPane.showMessageDialog(null, e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
		}
	}


	public static void write(String filename, ByteBuffer buffer) {
        try {
            RandomAccessFile store = new RandomAccessFile(filename, "rw");
            FileChannel channel = store.getChannel();

			channel.write(buffer);
            channel.close();
            store.close();
        } catch (Exception e) {
            JOptionPane.showMessageDialog(null, e.toString(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

	public static ByteBuffer read(String filename) {
        try {
            RandomAccessFile store = new RandomAccessFile(filename, "r");
            FileChannel channel = store.getChannel();

			ByteBuffer buffer = ByteBuffer.allocate(64);

			channel.read(buffer);
            channel.close();
            store.close();

			buffer.rewind();
			return buffer;
        } catch (Exception e) {
            return null;
        }
    }

	public static String readString(String filename) throws Exception {
		return new Scanner(new File(filename)).useDelimiter("\\Z").next();
	}
}
