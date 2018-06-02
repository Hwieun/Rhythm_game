#ifndef VIDEO
#define VIDEO
#include <includes.h>
#endif

//#define TRACK_BAR
#define YCbCr
//#define HSV
#define Detecting 1
#define Tracking 2


#ifdef TRACK_BAR
static void getThreshold(int, void*)
{

}
#endif

class Video {
private:
	/*Common*/
	VideoCapture camera;
	Size sz;
	string str;
	float fps;
	Rect ROI[2];
	Point centerOfHand;
	bool isClicked;
	double scale;

	/*Detecting*/
	// @origin : 사용자에게 보여질 영상
	// @hand : 영상처리용 matrix, 이진화된 손영상, origin과 같은 사이즈
	// @handROI : hand의 일부영역, 실제로 영상처리가 되는 영역
	// @originROI : oringin의 일부영역, 영상처리가 된 결과가 그려지는 영역. handROI 같은 사이즈.
	Mat origin, hand, handROI, originROI;
	// @diff : origin이랑 backup이랑 뺀값.
	// @backup : origin이전 영상(이미지).
	Mat diff, backup;
	// @preFingerCount : 이전손가락 갯수.
	// @largestIndex : hand contour들 중 제일 큰 contour의 index
	int preFingerCount, largestIndex;
	vector<vector<Point>> contours, circleContour;
	double radius;

	/*Tracking*/
	// @state : x,y,v_x,v_y,w,h
	// @measure : x,y,w,h
	Mat state, measure;
	KalmanFilter kf;
	bool foundHand;
	int notFoundCount;
	int precTick;

#ifdef TRACK_BAR
	int tb[3][2];
#endif
public:
	Video() {
		sz.width = 1280, sz.height = 720;
		ROI[0] = Rect(700, 120, 500, 500);
		isClicked = false;
		foundHand = false;

		int stateSize = 6;
		int measureSize = 4;
		int controlSize = 0;
		unsigned int type = CV_32F;
		kf = KalmanFilter(stateSize, measureSize, controlSize, type);

		state = Mat(stateSize, 1, type);  // [x,y,v_x,v_y,w,h]
		measure = Mat(measureSize, 1, type);    // [z_x,z_y,z_w,z_h]
												//cv::Mat procNoise(stateSize, 1, type)
												// [E_x,E_y,E_v_x,E_v_y,E_w,E_h]

												// Transition State Matrix A
												// Note: set dT at each processing step!
												// [ 1 0 dT 0  0 0 ]
												// [ 0 1 0  dT 0 0 ]
												// [ 0 0 1  0  0 0 ]
												// [ 0 0 0  1  0 0 ]
												// [ 0 0 0  0  1 0 ]
												// [ 0 0 0  0  0 1 ]
		setIdentity(kf.transitionMatrix);

		// Measure Matrix H
		// [ 1 0 0 0 0 0 ]
		// [ 0 1 0 0 0 0 ]
		// [ 0 0 0 0 1 0 ]
		// [ 0 0 0 0 0 1 ]
		kf.measurementMatrix = Mat::zeros(measureSize, stateSize, type);
		kf.measurementMatrix.at<float>(0) = 1.0f;
		kf.measurementMatrix.at<float>(7) = 1.0f;
		kf.measurementMatrix.at<float>(16) = 1.0f;
		kf.measurementMatrix.at<float>(23) = 1.0f;

		// Process Noise Covariance Matrix Q
		// [ Ex   0   0     0     0    0  ]
		// [ 0    Ey  0     0     0    0  ]
		// [ 0    0   Ev_x  0     0    0  ]
		// [ 0    0   0     Ev_y  0    0  ]
		// [ 0    0   0     0     Ew   0  ]
		// [ 0    0   0     0     0    Eh ]
		//setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
		kf.processNoiseCov.at<float>(0) = 1e-2;
		kf.processNoiseCov.at<float>(7) = 1e-2;
		kf.processNoiseCov.at<float>(14) = 5.0f;
		kf.processNoiseCov.at<float>(21) = 5.0f;
		kf.processNoiseCov.at<float>(28) = 1e-2;
		kf.processNoiseCov.at<float>(35) = 1e-2;

		// Measures Noise Covariance Matrix R
		setIdentity(kf.measurementNoiseCov, Scalar(1e-1));
		// <<<< Kalman Filter

		//camera = VideoCapture(1);
		camera = VideoCapture("1_banpal_hanson_color.avi");
		if (!camera.isOpened())
			cout << "Camera not connected!" << endl;
#ifdef TRACK_BAR
#ifdef YCbCr
		namedWindow("YCbCr", WINDOW_AUTOSIZE);
		createTrackbar("y_min", "YCbCr", &tb[0][0], 255, getThreshold);
		setTrackbarPos("y_min", "YCbCr", 0);
		createTrackbar("y_max", "YCbCr", &tb[0][1], 255, getThreshold);
		setTrackbarPos("y_max", "YCbCr", 255);

		createTrackbar("cb_min", "YCbCr", &tb[1][0], 255, getThreshold);
		setTrackbarPos("cb_min", "YCbCr", 0);
		createTrackbar("cb_max", "YCbCr", &tb[1][1], 255, getThreshold);
		setTrackbarPos("cb_max", "YCbCr", 255);

		createTrackbar("cr_min", "YCbCr", &tb[2][0], 255, getThreshold);
		setTrackbarPos("cr_min", "YCbCr", 0);
		createTrackbar("cr_max", "YCbCr", &tb[2][1], 255, getThreshold);
		setTrackbarPos("cr_max", "YCbCr", 255);
#endif
#ifdef HSV
		namedWindow("HSV", WINDOW_AUTOSIZE);
		createTrackbar("h_min", "HSV", &tb[0][0], 255, getThreshold);
		setTrackbarPos("h_min", "HSV", 0);
		createTrackbar("h_max", "HSV", &tb[0][1], 255, getThreshold);
		setTrackbarPos("h_max", "HSV", 255);

		createTrackbar("s_min", "HSV", &tb[1][0], 255, getThreshold);
		setTrackbarPos("s_min", "HSV", 0);
		createTrackbar("s_max", "HSV", &tb[1][1], 255, getThreshold);
		setTrackbarPos("s_max", "HSV", 255);

		createTrackbar("v_min", "HSV", &tb[2][0], 255, getThreshold);
		setTrackbarPos("v_min", "HSV", 0);
		createTrackbar("v_max", "HSV", &tb[2][1], 255, getThreshold);
		setTrackbarPos("v_max", "HSV", 255);
#endif
#endif
	};
	void ReadCam();
	void HandDetect();
	void HandTracking();
	void Drawing(int level, int flag);
	void Close();
	void MakeTrackBar();
	void GetTrackBar();
};