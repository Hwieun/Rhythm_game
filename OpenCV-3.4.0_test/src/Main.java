import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;

public class Main {
	static {
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
	}
	static Mat image = null;

	public static void main(String[] args) {
		JFrame frame = new JFrame("MOTION DETECTOR");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		JLabel vidpanel = new JLabel();
		frame.setContentPane(vidpanel);
		frame.setSize(1280, 670);
		frame.setVisible(true);

		Mat m_frame = new Mat();
		Mat change = new Mat();
		Mat outBox = new Mat();
		Mat diff_frame = null;
		Mat temp_frame = null;
		ArrayList<Rect> array = new ArrayList<Rect>(); //검출하는 네모박스를 저장하는 배열
		Size sz = new Size(1280, 670);
		VideoCapture camera = new VideoCapture("C:\\Users\\Kim Hwieun\\Desktop\\kkk\\test.mp4");
		int i = 0;

		while (true) {
			if (camera.read(m_frame)) {
				Core.flip(m_frame, change, 1); //좌우반전해서 change에 저장
				Imgproc.resize(change, change, sz);
				image = change.clone(); //image에 change 배열 복제
				outBox = new Mat(sz, CvType.CV_8SC1); //8bit signed char type
				Imgproc.cvtColor(change, outBox, Imgproc.COLOR_BGR2GRAY); //grayscale로 변환 후 outBox에 저장
				Imgproc.GaussianBlur(outBox, outBox, new Size(3, 3), 0); //Gaussian noise blur

				if (i == 0) {
					frame.setSize(change.width(), change.height()); //JFrame의 사이즈를 영상에 맞춤
					diff_frame = new Mat(outBox.size(), CvType.CV_8UC1);
					temp_frame = new Mat(outBox.size(), CvType.CV_8UC1);
					diff_frame = outBox.clone();
				}
				
				if (i == 1) {
//					Core.subtract(outBox, temp_frame, diff_frame);  //diff_frame = outBox - temp_frame diff_frame에는 outBox와 temp_frame이 다른 부분만 흰색으로 그려진다
					Imgproc.adaptiveThreshold(diff_frame, diff_frame, 255, Imgproc.ADAPTIVE_THRESH_MEAN_C,
							Imgproc.THRESH_BINARY_INV, 5, 2); //선을 그려줌. 차분한 후의 값이 threshold이상인 부분이 에지가 된다.
					array = detection(diff_frame); //윤곽의 배열을 구함
					if (array.size() > 0) { //배열안의 Rect를 그림
						Iterator<Rect> it2 = array.iterator();
						while (it2.hasNext()) { //다음 배열이 있을때 반복문실행
							Rect obj = it2.next();
							Imgproc.rectangle(image, obj.br(), obj.tl(), new Scalar(0, 255, 0), 1); //image위에 좌표, 좌표에 green색으로 1의 굵기로 네모를 그림
						}
					}
				}

				i = 1;

				ImageIcon img = new ImageIcon(MatTobufferedImage(image)); //Mat을 ImageIcon으로 바꿈
				vidpanel.setIcon(img); //JLabel에 img라는ImageIcon을 올린다
				vidpanel.repaint(); //JLabel 그림
				temp_frame = outBox.clone(); 
			}
		} 
	}

	public static BufferedImage MatTobufferedImage(Mat image) {
		MatOfByte bytemat = new MatOfByte();
		Imgcodecs.imencode(".jpg", image, bytemat); //image영상을 bytemat 버퍼에 jpg형식으로 인코딩
		byte[] bytes = bytemat.toArray();
		InputStream in = new ByteArrayInputStream(bytes);
		BufferedImage img = null;
		try {
			img = ImageIO.read(in);
		} catch (IOException e) {
			e.printStackTrace();
		}
		return img;
	}

	public static ArrayList<Rect> detection(Mat outmat) { //변한 부분만 나오는 이미지(diff_frame)의 윤곽을 잡아서 Rect형식의 배열에 넣어서 반환
		Mat v = new Mat();
		Mat v2 = outmat.clone();
		List<MatOfPoint> contours = new ArrayList<MatOfPoint>(); //contours == 윤곽. contour는 List<MatOfPoint>형으로 선언해야함
		Imgproc.findContours(v2, contours, v, Imgproc.RETR_LIST, Imgproc.CHAIN_APPROX_SIMPLE); 
		//Imgproc.findContours(src image, detected contours, hierarchy) //contours이라는 list검색
		 // CV_CHAIN_APPROX_SIMPLE 플래그로 하면 마지막 점이 수평 또는 수직, 대각선 외곽선에 포함됨
		double maxArea = 100;
		int maxAreaIdx = -1;
		Rect r = null;

		ArrayList<Rect> rectArray = new ArrayList<Rect>();

		for (int idx = 0; idx < contours.size(); idx++) {
			Mat contour = contours.get(idx);
			double contourarea = Imgproc.contourArea(contour);
			if (contourarea > maxArea) {
				maxAreaIdx = idx;
				r = Imgproc.boundingRect(contours.get(maxAreaIdx));
				rectArray.add(r);
			}
		}
		v.release();

		return rectArray;
	}
	
}