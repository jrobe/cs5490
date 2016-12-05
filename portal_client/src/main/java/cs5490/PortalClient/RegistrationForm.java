package cs5490.PortalClient;

import java.util.*;
import javax.swing.JFrame;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import layout.*;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TTransport;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.protocol.TProtocol;
import org.apache.thrift.TException;

import cs.*;

public class RegistrationForm extends JFrame implements ActionListener {
	private java.util.List<Row> rows = new ArrayList<>();
	private AuthServe.Client client;

	public RegistrationForm(AuthServe.Client client) {
		this.client = client;

		setTitle("Portal Client - New User Registration");
        setSize(400, 175);
        setDefaultCloseOperation(HIDE_ON_CLOSE);

		JPanel panel = getRegisterForm();
		add(panel);
		setVisible(true);
	}

	public void actionPerformed(ActionEvent event) {
		Map<String, String> fields = new HashMap<>();
		String username = null;
		for (Row row : rows) {
			if (row.label == "Username") {
				username = row.textField.getText();
			} else {
				fields.put(row.label, row.textField.getText());
			}
		}
		
		if (username != null) {
			try {
				App.write(App.keyFilename,
					client.createAccount(username, fields));

                    // Close the window after registering...
                    setVisible(false);
                    dispatchEvent(new WindowEvent(this, WindowEvent.WINDOW_CLOSING));
			} catch (error e) {
				JOptionPane.showMessageDialog(this, e.getReason(), "Error Creating Account", JOptionPane.ERROR_MESSAGE);
			} catch (TException e) {
				JOptionPane.showMessageDialog(this, e.toString(), "Communication Error", JOptionPane.ERROR_MESSAGE);
                dispose();
			}
		}
	}

	private JPanel getRegisterForm() {
		JPanel mainPanel = new JPanel();
		mainPanel.setLayout(new BorderLayout());

		JButton registerButton = new JButton("Register");
		mainPanel.add(registerButton, BorderLayout.PAGE_END);
		registerButton.addActionListener(this);


		String[] labels = {"Username", "Email", "Phone Number", "Credit Card"};
		JPanel formPanel = new JPanel();
		formPanel.setLayout(new SpringLayout());

		for (String s : labels) {
			JLabel label = new JLabel(s+":", SwingConstants.RIGHT);
			formPanel.add(label);
			JTextField textField = new JTextField();
			formPanel.add(textField);

			this.rows.add(new Row(s, textField));
		}

		SpringUtilities.makeCompactGrid(formPanel, labels.length, 2, // rows, cols
													5, 5, // start x, y
													5, 5); // padding x, y

		mainPanel.add(formPanel, BorderLayout.CENTER);

		return mainPanel;
	}

	private class Row {
		public final String label;
		public final JTextField textField;

		public Row(String label, JTextField textField) {
			this.label = label;
			this.textField = textField;
		}
	}
}
