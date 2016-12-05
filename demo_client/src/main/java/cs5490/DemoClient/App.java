package cs5490.DemoClient;

import java.awt.EventQueue;

import org.apache.thrift.TException;
import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;

import cs.*;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
import javax.swing.JSeparator;
import javax.swing.JTextField;
import javax.swing.JButton;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import javax.swing.JProgressBar;

public class App implements RequestCheckCallback, LogoutCallback{
	/**
	 * 
	 */
	private JFrame frame;
	private JTextField userNameField;
	private JButton submitButton;
	private JProgressBar progressBar;
	private Boolean waiting = false;
	private RequestCheck requestRunnable;
	private Thread requestThread;
	private JLabel approvalLabel;
	private SuccessfulLoginPanel successfulLoginPanel;
	private JLabel userNamePromptLabel;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					App window = new App();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}
	
	public long logIn(String username)
	{
		PermissionRequest pr = null;
		try
		{
			TTransport transport;
			TSSLTransportParameters params = new TSSLTransportParameters();
			params.setTrustStore("../ssl/truststore.jks", "cs5490");
			transport = TSSLTransportFactory.getClientSocket("localhost", 9090, 0, params);
			TProtocol protocol = new  TBinaryProtocol(transport);
			AuthServe.Client client = new AuthServe.Client(protocol);
			pr = client.requestPermission(username);
			transport.close();
			userNameField.setEditable(false);
			userNameField.setEnabled(false);
			submitButton.setText("Cancel");
			progressBar.setVisible(true);
			approvalLabel.setVisible(true);
			waiting = true;
			
			requestRunnable = new RequestCheck(this, pr.requestID);
			requestThread = new Thread(requestRunnable);
			requestThread.start();
			
			return pr.requestID;
		}
		catch (TException e)
		{
			//e.printStackTrace();
			if(e.toString().contains("user doesn't exist"))
				approvalLabel.setText("No user with that name exists.");
			else
				approvalLabel.setText(e.toString());
			approvalLabel.setVisible(true);
			frame.repaint();
			return -1;
		}
	}

	public void requestApproved(PermissionRequest pr) {
		if(waiting)
		{
			waiting=false;
			progressBar.setVisible(false);
			approvalLabel.setVisible(false);
			submitButton.setText("Submit");
			userNameField.setEditable(true);
			userNameField.setEnabled(true);
			userNameField.setText("");

			userNamePromptLabel.setVisible(false);
			userNameField.setVisible(false);
			submitButton.setVisible(false);
			successfulLoginPanel = new SuccessfulLoginPanel(pr, this);
			successfulLoginPanel.setBounds(10, 37, 496, 361);
			frame.add(successfulLoginPanel);
		}
	}

	public void requestFailed(String reason) {
		waiting=false;
		progressBar.setVisible(false);
		approvalLabel.setText(reason);
		submitButton.setText("Submit");
		userNameField.setEditable(true);
		userNameField.setEnabled(true);
	}
	
	public void logout() {
		frame.remove(successfulLoginPanel);
		successfulLoginPanel = null;
		waiting=false;
		userNamePromptLabel.setVisible(true);
		userNameField.setVisible(true);
		submitButton.setVisible(true);
	}
	
	/**
	 * Create the application.
	 */
	public App() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frame = new JFrame();
		frame.setResizable(false);
		frame.setBounds(100, 100, 518, 426);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.getContentPane().setLayout(null);
		
		JLabel lblDemoApp = new JLabel("CS 5490 - Demo Application");
		lblDemoApp.setBounds(10, 10, 496, 16);
		lblDemoApp.setHorizontalAlignment(SwingConstants.CENTER);
		frame.getContentPane().add(lblDemoApp);
		
		JSeparator separator = new JSeparator();
		separator.setBounds(10, 26, 496, 12);
		frame.getContentPane().add(separator);
		
		userNamePromptLabel = new JLabel("Please Enter your Username to log in...");
		userNamePromptLabel.setBounds(138, 110, 242, 16);
		frame.getContentPane().add(userNamePromptLabel);
		
		userNameField = new JTextField();
		userNameField.setBounds(138, 132, 242, 28);
		frame.getContentPane().add(userNameField);
		userNameField.setColumns(10);
		
		submitButton = new JButton("Submit");
		submitButton.setBounds(292, 166, 88, 29);
	
		submitButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if(!waiting)
				{
					approvalLabel.setText("Waiting For Approval...");
					approvalLabel.setVisible(false);
					logIn(userNameField.getText());
				}
				else
				{
					requestRunnable.shutdown = true;
					waiting = false;
					progressBar.setVisible(false);
					approvalLabel.setVisible(false);
					submitButton.setText("Submit");
					userNameField.setEditable(true);
					userNameField.setEnabled(true);
				}
				
			}
		});
		frame.getContentPane().add(submitButton);
		
		progressBar = new JProgressBar();
		progressBar.setBounds(138, 166, 146, 20);
		progressBar.setIndeterminate(true);
		frame.getContentPane().add(progressBar);
		progressBar.setVisible(false);
		
		approvalLabel = new JLabel("Waiting for Approval...");
		approvalLabel.setHorizontalAlignment(SwingConstants.CENTER);
		approvalLabel.setBounds(10, 198, 496, 16);
		frame.getContentPane().add(approvalLabel);
		approvalLabel.setVisible(false);
	}

}
