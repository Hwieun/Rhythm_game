package Packone;

import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;

public class Video extends Thread {
	static Mat image = null;
	private VideoCapture camera;
	private Mat origin;
	private Size sz;

	public Video() {
		origin = new Mat();
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
			Core.flip(origin, origin, 1); // y축 기준으로 뒤집기
			Imgproc.resize(origin, origin, sz);
			image = origin.clone();

			ImageIcon cImg = new ImageIcon(Mat2BufferedImage(image));
			return cImg.getImage();
		}
		return null;
	}
	
	public void close(){
		camera.release();
		this.interrupt();
	}
}