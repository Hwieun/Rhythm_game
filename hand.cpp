#include "stdafx.h"

#define Detecting 1
#define Tracking 2
#define VIDEO1 "1_banpal_hanson_color.avi"
#define VIDEO2 "2_banpal_hanson_click_tracking.avi"
#define VIDEO3 "3_banpal_dooson_click_tracking_hide.avi"
#define VIDEO4 "4_ginpal_dooson_color.avi"
#define VIDEO5 "5_ginpal_dooson_click_tracking_hide.avi"

#define VIDEO6 "sample_noBox_1.mp4"
#define VIDEO7 "sample_noBox_2.mp4"
#define VIDEO8 "sample_noBox_3.mp4"
#define VIDEO9 "sample_noBox_4.mp4"

using namespace cv;
using namespace std;

class Video {
private:
	/*Common*/
	VideoCapture camera;
	Size sz;
	string str;
	float fps;
	clock_t curTime;
	Rect ROI[2];
	Point centerOfHand;
	bool isClicked;
	bool qt_flag;

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
	double precTick;

public:
	Video() {
		largestIndex = -1;
		sz.width = 1280, sz.height = 720;
		ROI[0] = Rect(700, 120, 500, 500);
		isClicked = false;
		foundHand = false;
		qt_flag = false;

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

		camera = VideoCapture(VIDEO6);
		if (!camera.isOpened())
			cout << "Camera not connected!" << endl;

	}

	void ReadCam() {
		time_t cur_time;
		struct tm cur_tm;
		cur_time = time(NULL);
		localtime_s(&cur_tm, &cur_time);
		VideoWriter sample;
		char name[20];
	//	sprintf_s(name, "%d_%d_%d.mp4", cur_tm.tm_mday, cur_tm.tm_hour, cur_tm.tm_min);
		sample.open("sample_HSV.mp4", VideoWriter::fourcc('X', 'V', 'I', 'D'), 18, sz, true);

		while (camera.read(origin))
		{
			Test_2();
			if (contours.size() > 0)
				Drawing(2, Detecting);
			//curTime = clock();

//			if (!isClicked) {
//				HandDetect();
				//if (contours.size() > 0)
				//	Drawing(1, Detecting);
				//fps = (float)(clock() - curTime) / CLOCKS_PER_SEC;
				//fps = 1 / fps;
				//cout << "fps : " << fps << endl;
			//}
			//else
			//   HandTracking();

			sample << origin;
//			imshow("main", hand);
			if (27 == waitKey(1)) Close();
		}
	}

	void Test_1() {
		resize(origin, origin, sz);
		flip(origin, origin, 1);
		cvtColor(origin, hand, COLOR_RGB2HSV);
//		inRange(hand, Scalar(0, 95, 135), Scalar(255, 125, 160), hand);
		inRange(hand, Scalar(0, 0.23 * 255, 0.13 * 255), Scalar(50, 0.68 * 255, 255), hand);

		cvtColor(hand, hand, COLOR_GRAY2RGB);
		imshow("YCrCb", hand);
	}

	void Test_2() {
		resize(origin, origin, sz);
		flip(origin, origin, 1);
		cvtColor(origin, hand, COLOR_RGB2HSV);
		inRange(hand, Scalar(0, 95, 135), Scalar(255, 125, 160), hand);
		Mat verticalStructure = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		erode(hand, hand, verticalStructure);
		erode(hand, hand, verticalStructure);
		Mat hierarchy;
		handROI = Mat(hand, ROI[0]);
		originROI = Mat(origin, ROI[0]);
		findContours(handROI, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE, Point(0, 0));
		rectangle(origin, ROI[0], Scalar(150, 150, 0), 3); //draw ROI
//		imshow("ROI", handROI);
		int bigsize = 0;

		if (contours.size() > 0) {
			for (int i = 0; i < contours.size(); i++) {
				double a = contourArea(contours[i], false);
				if (a > bigsize) {
					bigsize = a;
					largestIndex = i;
				}
			}
			if (contours.size() != 0) { //중심점, 손가락
				Mat center = Mat::zeros(Size(ROI[0].width, ROI[0].height), CV_8UC1);
				Mat distChange = center.clone();
				drawContours(center, contours, largestIndex, Scalar(255), -1);
				distanceTransform(center, distChange, CV_DIST_L2, 5);
				int maxIdx[2];    //좌표 값을 얻어올 배열(행, 열 순으로 저장됨)
				minMaxIdx(distChange, NULL, &radius, NULL, maxIdx, center);   //최소값은 사용 X
				centerOfHand = Point(maxIdx[1], maxIdx[0]);

				if (centerOfHand.x > 0 && centerOfHand.y > 0 && centerOfHand.x < ROI[0].width && centerOfHand.y < ROI[0].height) {
					str.clear();
					str += "radius : ";
					str += to_string(radius);
					putText(origin, str, Point(10, 75), 0, 1, Scalar(0, 0, 255), 2);
				}
			}
		}
	}

	void Video::HandDetect() {
		double scale = 1.8;
		qt_flag = false;
		Mat hierarchy;

		resize(origin, origin, sz);
		flip(origin, origin, 1);
		cvtColor(origin, hand, COLOR_RGB2YCrCb);
		inRange(hand, Scalar(0, 95, 135), Scalar(255, 125, 160), hand);

		//#Q1.kernel size
		//#Q2.morphology sequence
		Mat verticalStructure = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		erode(hand, hand, verticalStructure);
		erode(hand, hand, verticalStructure);

		handROI = Mat(hand, ROI[0]);
		originROI = Mat(origin, ROI[0]);
		findContours(handROI, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE, Point(0, 0));
		rectangle(origin, ROI[0], Scalar(150, 150, 0), 3); //draw ROI
//		imshow("ROI", handROI);
		int bigsize = 0;

		if (contours.size() > 0) {
			for (int i = 0; i < contours.size(); i++) {
				double a = contourArea(contours[i], false);
				if (a > bigsize) {
					bigsize = a;
					largestIndex = i;
				}
			}
			if (contours[largestIndex].size() < 20) contours.clear();

			if (contours.size() != 0) { //중심점, 손가락
				Mat center = Mat::zeros(Size(ROI[0].width, ROI[0].height), CV_8UC1);
				Mat distChange = center.clone();
				Mat cImg = center.clone();
				drawContours(center, contours, largestIndex, Scalar(255), -1);
				distanceTransform(center, distChange, CV_DIST_L2, 5);
				int maxIdx[2];    //좌표 값을 얻어올 배열(행, 열 순으로 저장됨)
				minMaxIdx(distChange, NULL, &radius, NULL, maxIdx, center);   //최소값은 사용 X
				centerOfHand = Point(maxIdx[1], maxIdx[0]);
			
				if (centerOfHand.x > 0 && centerOfHand.y > 0 && centerOfHand.x < ROI[0].width && centerOfHand.y < ROI[0].height) {
					str.clear();
					str += "radius : ";
					str += to_string(radius);
					putText(origin, str, Point(10, 75), 0, 1, Scalar(0, 0, 255), 2);
					circle(cImg, centerOfHand, (int)(radius*scale), Scalar(255), 3);
					cImg = cImg & center;
					//imshow("cImg", cImg);
					findContours(cImg, circleContour, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

					int fingercount = 0, thre = 5;
					for (int i = 0; i < circleContour.size(); i++)
						if (circleContour[i].size() > thre) fingercount++;
					if (preFingerCount - fingercount >= 4) {
						putText(origin, "click event!!", Point(10, 45), 0, 1, Scalar(0, 0, 255), 2);
						printf("click event!!!!!\n");
						qt_flag = true;
						isClicked = true;                          
					}
					str = "finger count : ";
					str += to_string(fingercount - 1);
					putText(origin, str, Point(10, 25), 0, 1, Scalar(0, 0, 255), 2);
					preFingerCount = fingercount;
				}
			}
		}
	}

	void Video::HandTracking() {
		double ticks = 0;
		ticks = (double)cv::getTickCount();
		double dT = (ticks - precTick) / getTickFrequency(); //seconds
		precTick = ticks;

		HandDetect();

		vector<Rect> handsBox;
		if (contours.size() > 0) {
			Rect hBox = boundingRect(contours[largestIndex]);

			//float ratio = (float)hBox.width / (float)hBox.height;
			//if (ratio > 1.0f)
			//	ratio = 1.0f / ratio;

			handsBox.push_back(hBox);
			drawContours(originROI, contours, largestIndex, CV_RGB(0, 0, 255), 2);
			rectangle(originROI, handsBox[0], CV_RGB(0, 255, 0), 2);
			circle(originROI, centerOfHand, 5, CV_RGB(0, 0, 255), 2);
			printf("center point :(%d, %d)\nROI :(%d, %d)\n", centerOfHand.x, centerOfHand.y, ROI[0].x, ROI[0].y);
		}
		// <<<<< Detection result Drawing

		if (handsBox.size() == 0) { ///여기서 에러발생
			notFoundCount++;
			cout << "notFoundCount:" << notFoundCount << endl;
			if (notFoundCount >= 30) {
				notFoundCount = 0;
				foundHand = false;	
				isClicked = false;
				ROI[0].x = 700, ROI[0].y = 120;
				ROI[0].width = 500;
				ROI[0].height = 500;
				return;
			}
		}
		// bounding에서 찾은 object가 없으면
		else {
			notFoundCount = 0;

			measure.at<float>(0) = centerOfHand.x + ROI[0].x;
			measure.at<float>(1) = centerOfHand.y + ROI[0].y;
			measure.at<float>(2) = (float)handsBox[0].width * 1.5;
			measure.at<float>(3) = (float)handsBox[0].height * 1.5;
		}

		if (!foundHand) {
			// >>>> Initialization
			kf.errorCovPre.at<float>(0) = 1; // px
			kf.errorCovPre.at<float>(7) = 1; // px
			kf.errorCovPre.at<float>(14) = 1;
			kf.errorCovPre.at<float>(21) = 1;
			kf.errorCovPre.at<float>(28) = 1; // px
			kf.errorCovPre.at<float>(35) = 1; // px

			state.at<float>(0) = measure.at<float>(0);
			state.at<float>(1) = measure.at<float>(1);
			state.at<float>(2) = 0;
			state.at<float>(3) = 0;
			state.at<float>(4) = measure.at<float>(2);
			state.at<float>(5) = measure.at<float>(3);
			// <<<< Initialization

			kf.statePost = state;
			foundHand = true;
		}
		else {
			if (notFoundCount == 0) {
				cout << "State Measure:" << endl << measure << endl;
				kf.transitionMatrix.at<float>(2) = dT;
				kf.transitionMatrix.at<float>(9) = dT;
				kf.correct(measure);
				printf("dT :%lf\n", dT);
			}
			else {
				kf.transitionMatrix.at<float>(2) = 0;
				kf.transitionMatrix.at<float>(9) = 0;
			}
			state = kf.predict();
			cout << "State post:" << endl << state << endl;
		}

		ROI[0].width = state.at<float>(4);
		ROI[0].height = state.at<float>(5);

		ROI[0].x = state.at<float>(0) - ROI[0].width / 2;
		if (ROI[0].x < 0) ROI[0].x = 0;
		if (ROI[0].x + ROI[0].width > sz.width) ROI[0].width = sz.width - ROI[0].x;
		if (ROI[0].x > sz.width || ROI[0].width < 0) {
			ROI[0].x = 700; ROI[0].width = 500;
		}
		if (ROI[0].width > sz.width) ROI[0].width = 1280;

		ROI[0].y = state.at<float>(1) - ROI[0].height / 2;
		if (ROI[0].y < 0) ROI[0].y = 0;
		if (ROI[0].y + ROI[0].height > sz.height) ROI[0].height = sz.height - ROI[0].y;
		if (ROI[0].y > sz.height || ROI[0].height < 0) {
			ROI[0].y = 120; ROI[0].height = 500;
		}
		if (ROI[0].height > sz.height) ROI[0].height = 720;
		//>>>>>ROI 재설정
		printf("Tracking : ROI.x = %d, ROI.y = %d, ROI.width = %d, ROi.height = %d\n", ROI[0].x, ROI[0].y, ROI[0].width, ROI[0].height);

		///predict 결과값 그리기
		Point draw_center(state.at<float>(0), state.at<float>(1));
		circle(originROI, draw_center, 3, CV_RGB(255, 0, 0), -1);
	
		//circle()

		stringstream sstr;
		sstr << "(" << draw_center.x << "," << draw_center.y << ")";
		putText(originROI, sstr.str(), Point(draw_center.x + 3, draw_center.y - 3), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(20, 150, 20), 2);
	}

	Point Video::GetPoint() {
		return centerOfHand;
	}

	void Video::Drawing(int level, int flag) {
		// contours centerOfHand radius circleContour
		//contour만
		if (level >= 1) {
			drawContours(originROI, contours, largestIndex, Scalar(0, 0, 255), 3); //skin color contours
		}
		//원3개. 중심점,손바닥원,손가락원
		if (level >= 2) {
			circle(originROI, centerOfHand, 2, Scalar(255, 255, 0), 3); //중심점 파란색
			circle(originROI, centerOfHand, (int)radius, Scalar(0, 0, 255), 3); //손바닥 빨간색
			circle(originROI, centerOfHand, (int)(radius * 2), Scalar(255, 0, 255), 3); //손가락 기준되는 원 보라색
			printf("center point :(%d, %d)\n", centerOfHand.x, centerOfHand.y);
		}
		//boxing(measure)
		if (level >= 3) {
		
		}
		return;
	}

	void Video::Close() {
		destroyAllWindows();
		camera.release();
	}
};

int main(int argc, _TCHAR* argv[])
{
	Video video;
	video.ReadCam();

	return 0;
}