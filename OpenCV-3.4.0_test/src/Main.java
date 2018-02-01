import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.awt.image.WritableRaster;
import java.nio.file.Paths;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.WindowConstants;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.videoio.VideoCapture;

public class Main {

	public static void main(String[] args) throws InterruptedException {
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
		String filePath = "C:\\Users\\Kim Hwieun\\Desktop\\kkk\\test.mp4";
		if (!Paths.get(filePath).toFile().exists()) {
			System.out.println("File " + filePath + " does not exist!");
			return;
		}
//		JLabel label = new JLabel();
		VideoCapture camera = new VideoCapture(filePath);

		if (!camera.isOpened()) {
			System.out.println("Error! Camera can't be opened!");
			return;
		}
		Mat mat = new Mat();
		String[] str = { "camera0", "camera1", "camera2", "camera3", "camera4", "camera5", "camera6", "camera7",
				"camera8", "camera9" };
		int i = 0, j =0;
		while (true) {
			if (camera.read(mat)) {
				System.out.println("Frame Obtained");
				System.out.println("Captured Frame Width " + mat.width() + " Height " + mat.height());
				if (i >= 20){
					Imgcodecs.imwrite("C:\\Users\\Kim Hwieun\\Desktop\\kkk\\" + str[j] + ".jpg", mat);
					j++;
				}
				System.out.println("OK");
				i++;	
				Thread.sleep(10);
			}
			if (i == 30)
				break;
		}
		camera.release();
	}

	private static BufferedImage matToBufferedImage(Mat frame) {
		int type = 0;
		if (frame.channels() == 1) {
			type = BufferedImage.TYPE_BYTE_GRAY;
		} else if (frame.channels() == 3) {
			type = BufferedImage.TYPE_3BYTE_BGR;
		}
		BufferedImage image = new BufferedImage(frame.width(), frame.height(), type);
		WritableRaster raster = image.getRaster();
		DataBufferByte dataBuffer = (DataBufferByte) raster.getDataBuffer();
		byte[] data = dataBuffer.getData();
		frame.get(0, 0, data);

		return image;
	}

	private static void showWindow(BufferedImage img) {
		JFrame frame = new JFrame();
		frame.getContentPane().add(new JLabel(new ImageIcon(img)));
		frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
		frame.setSize(img.getWidth(), img.getHeight() + 30);
		frame.setTitle("Image captured");
		frame.setVisible(true);
	}
	/*
	 * public void paint(Graphics g){ Image screenImage; screenImage =
	 * frame.createImage(SCREEN_WIDTH, SCREEN_HEIGHT); screenGraphics =
	 * screenImage.getGraphics(); screenDraw((Graphics2D) screenGraphics);
	 * g.drawImage(screenImage, 0, 0, null); } public void screenDraw(Graphics2D
	 * g, Image img) { g.drawImage(img, 0, 0, null); }
	 */
}