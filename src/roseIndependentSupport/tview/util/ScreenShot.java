package util;



import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.imageio.*;
import javax.swing.JFrame;


public class ScreenShot {
	static int counter=0;
	public static void createScreenShot (long sleeptime, String outFileName, JFrame frame) throws Exception {
		// check if file name is valid
//		outFileName=outFileName+(counter++)+ ".png";
		outFileName=outFileName+ ".png";
		// wait for a user-specified time
		try {
			long time = sleeptime * 1000L;
//			System.out.println("Waiting " + (time / 1000L) + 
//				" second(s)...");
			Thread.sleep(time);
		} catch(NumberFormatException nfe) {
			System.err.println(sleeptime + " does not seem to be a " +
				"valid number of seconds.");
			System.exit(1);
		}
		// determine current screen size
		Toolkit toolkit = Toolkit.getDefaultToolkit();
		
		Rectangle screenRect = frame.getBounds();
//		Dimension screenSize = toolkit.getScreenSize();
//		Rectangle screenRect = new Rectangle(screenSize);
		// create screen shot
		Robot robot = new Robot();
		BufferedImage image = robot.createScreenCapture(screenRect);
		// save captured image to PNG file
		ImageIO.write(image, "png", new File(outFileName));
		// give feedback
		System.out.println("Saved screen shot (" + image.getWidth() +
			" x " + image.getHeight() + " pixels) to file \"" +
			outFileName + "\".");
		// use System.exit if the program hangs after writing the file;
		// that's an old bug which got fixed only recently
		// System.exit(0); 
	}
}

