package cs5490.DemoClient;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javax.swing.JPanel;

public class ImagePanel extends JPanel{

    /**
	 * 
	 */
	private static final long serialVersionUID = -7118041921709953948L;
	private BufferedImage image;

    public ImagePanel(String imageName) {
       try {                
          image = ImageIO.read(new File(imageName));
       } catch (IOException ex) {
            // handle exception...
    	   ex.printStackTrace();
       }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.drawImage(image, 0, 0, 263, 171, this);
    }

}