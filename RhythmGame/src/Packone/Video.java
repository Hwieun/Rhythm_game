package Packone;

import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;

public class Video extends Thread {
	private VideoCapture camera;
	private Mat origin, hand, labels;
	private Size sz;

	public Video() {
		origin = new Mat();
		hand = new Mat();
		labels = new Mat();
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
			ImageIcon cImg = new ImageIcon(Mat2BufferedImage(origin));
			return cImg.getImage();
		}
		return null;
	}

	public void handDetect() {
		List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
		List<Point[]> hullPoints = new ArrayList<Point[]>();
		List<MatOfPoint> hullMOP = new ArrayList<MatOfPoint>();
		Mat hierarchy = new Mat();
		Mat drawing = new Mat();
		int threshold1 = 85;
		Scalar skincolorLower = new Scalar(0, 0.23 * 255, 32);
		Scalar skincolorUpper = new Scalar(50, 0.68 * 255, 255);
		Imgproc.resize(origin, origin, sz);
		Core.flip(origin, origin, 1); // y축 기준으로 뒤집기
		Imgproc.cvtColor(origin, hand, Imgproc.COLOR_BGR2HSV);
		Core.inRange(hand, skincolorLower, skincolorUpper, hand);
		// Imgproc.blur(hand, hand, new Size(5, 5));
		// Imgproc.threshold(hand, hand, 200, 255, Imgproc.THRESH_BINARY);
		Mat verticalStructure = Imgproc.getStructuringElement(Imgproc.MORPH_RECT, new Size(1, hand.rows() / 50));
		Imgproc.erode(hand, hand, verticalStructure);
		Imgproc.erode(hand, hand, verticalStructure);
		Imgproc.dilate(hand, hand, verticalStructure);
		Imgproc.dilate(hand, hand, verticalStructure);
		Imgproc.findContours(hand, contours, hierarchy, Imgproc.RETR_EXTERNAL, Imgproc.CHAIN_APPROX_SIMPLE);
		Point centerOfHand = new Point();
		List<MatOfInt> hull = new ArrayList<MatOfInt>(contours.size());
		for (int i = 0; i < contours.size(); i++) {
			hull.add(new MatOfInt());
		}
		for (int i = 0; i < contours.size(); i++) {
			Imgproc.convexHull(contours.get(i), hull.get(i), false);
		}
		for (int i = 0; i < contours.size(); i++) {
			Point[] points = new Point[hull.get(i).rows()];

			// Loop over all points that need to be hulled in current contour
			for (int j = 0; j < hull.get(i).rows(); j++) {
				int index = (int) hull.get(i).get(j, 0)[0];
				points[j] = new Point(contours.get(i).get(index, 0)[0], contours.get(i).get(index, 0)[1]);
			}
			hullPoints.add(points);
		}

		// Convert Point arrays into MatOfPoint
		for (int i = 0; i < hullPoints.size(); i++) {
			MatOfPoint mop = new MatOfPoint();
			mop.fromArray(hullPoints.get(i));
			hullMOP.add(mop);
		}

		for (int i = 0; i < contours.size(); i++) {
			Imgproc.drawContours(origin, contours, i, new Scalar(0, 0, 255), 3);
			Imgproc.drawContours(origin, hullMOP, i, new Scalar(0, 255, 255), 3);
		}
		
		contours.clear();
	}

	public void close() {
		camera.release();
		this.interrupt();
	}
}