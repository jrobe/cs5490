package cs5490.DemoClient;

import java.awt.Panel;
import java.util.Map;

import cs.*;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JButton;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

interface LogoutCallback {
    void logout();
}

public class SuccessfulLoginPanel extends Panel {
	
	LogoutCallback c; 
	/**
	 * 
	 */
	private static final long serialVersionUID = -4220260293462075679L;

	SuccessfulLoginPanel(PermissionRequest pr, LogoutCallback cb)
	{
		c = cb;
		setLayout(null);
		
		JLabel successfulLoginLabel = new JLabel("Logged in successfully!");
		successfulLoginLabel.setBounds(170, 6, 155, 16);
		add(successfulLoginLabel);
		
		JLabel lblUserName = new JLabel("User Name:");
		lblUserName.setBounds(6, 37, 72, 16);
		add(lblUserName);
		
		JLabel lblEmailAddress = new JLabel("Email Address:");
		lblEmailAddress.setBounds(6, 62, 93, 16);
		add(lblEmailAddress);
		
		JLabel lblName = new JLabel("Phone #:");
		lblName.setBounds(6, 90, 54, 16);
		add(lblName);
		
		JPanel panel = new ImagePanel("src/main/resources/loggedIn.jpg");
		panel.setBounds(116, 184, 263, 171);
		add(panel);
		
		JLabel userNameLabel = new JLabel("");
		userNameLabel.setBounds(123, 37, 367, 16);
		add(userNameLabel);
		
		JLabel emailAddressLabel = new JLabel("");
		emailAddressLabel.setBounds(123, 62, 367, 16);
		add(emailAddressLabel);
		
		JLabel phoneLabel = new JLabel("");
		phoneLabel.setBounds(123, 90, 367, 16);
		add(phoneLabel);
		
		JLabel lblCreditCard = new JLabel("Credit Card #:");
		lblCreditCard.setBounds(6, 118, 93, 16);
		add(lblCreditCard);
		
		JLabel creditCardLabel = new JLabel("");
		creditCardLabel.setBounds(123, 118, 367, 16);
		add(creditCardLabel);
		
		Map<String, String> results = pr.results;
		creditCardLabel.setText(results.get("Credit Card"));
		phoneLabel.setText(results.get("Phone Number"));
		emailAddressLabel.setText(results.get("Email"));
		userNameLabel.setText(pr.user);
		
		JButton btnNewButton = new JButton("Logout");
		btnNewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				c.logout();
			}
		});
		btnNewButton.setBounds(185, 146, 117, 29);
		add(btnNewButton);
		this.repaint();
	}
}
