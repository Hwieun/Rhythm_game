package Packone;

import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.sql.Blob;
import java.util.ArrayList;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;

public class Video extends Thread {
	private VideoCapture camera;
	private Mat origin, hand;
	private Size sz;

	public Video() {
		origin = new Mat();
		hand = new Mat();
		sz = new Size(1280, 720);
		camera = new VideoCapture(0);
		if (!camera.isOpened()) {
			System.out.println("Camera not connected!");
		}
	}

	public static BufferedImage Mat2BufferedImage(Mat image) {
		BufferedImage result = null;
		MatOfByte bytemat = new MatOfByte();
		Imgcodecs.imencode(".jpg", image, bytemat);
		byte[] bytes = bytemat.toArray();
		InputStream in = new ByteArrayInputStream(bytes);
		try {
			result = ImageIO.read(in);
		} catch (IOException e) {
			e.printStackTrace();
		}
		return result;
	}

	public Image readCam() {
		if (camera.read(origin)) { // 읽어서 Mat타입으로 저장
			handDetect();
			ImageIcon cImg = new ImageIcon(Mat2BufferedImage(hand));
			return cImg.getImage();
		}
		return null;
	}

	public void handDetect() {
		List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
		// List<MatOfPoint> hull = new ArrayList<Mat>(contours.size());
		Mat hierarchy = new Mat();

		int threshold1 = 85;
		Scalar skincolorLower = new Scalar(0, 0.2 * 255, 0.25 * 255);
		Scalar skincolorUpper = new Scalar(15, 255, 0.8 * 255);
		Imgproc.resize(origin, origin, sz);
		Core.flip(origin, origin, 1); // y축 기준으로 뒤집기
		Imgproc.cvtColor(origin, hand, Imgproc.COLOR_BGR2HLS);
		Core.inRange(hand, skincolorLower, skincolorUpper, hand);
		Imgproc.blur(hand, hand, new Size(5, 5));
		Imgproc.threshold(hand, hand, 200, 255, Imgproc.THRESH_BINARY);
		Imgproc.findContours(hand, contours, hierarchy, Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_SIMPLE);

		for (int i = 1; i < contours.size(); i++)	
			Imgproc.drawContours(origin, contours, i, new Scalar(0, 0, 255), 2, 8, hierarchy, 0, new Point());
		

	}

	// public void _labeling(const Mat img, int threshold, int){}

	public void close() {
		camera.release();
		this.interrupt();
	}
}