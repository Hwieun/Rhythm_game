#include "stdafx.h"

using namespace cv;
using namespace std;

class Video {
private:
	VideoCapture camera;
	Mat origin, hand, diff, backUp, imgROI, drawROI;
	Size sz;
	//float fps, curTime;
	string str;
	int pre_fingercount;
	Rect ROI[2];
	Point centerOfHand;
	bool sethand;
public:
	Video() {
		sethand = false;
		sz.width = 1280;
		sz.height = 720;
		ROI[0] = Rect(700, 120, 550, 500);
		//		ROI[1] = Rect(700, 120, 550, 500);
		camera = VideoCapture(0);
		if (!camera.isOpened())
			printf("Camera not connected!");
	}

	void readCam() {
		char ch = 0;
		//char init = 0;

		while (camera.read(origin))
		{
			//curTime = clock();
			//if (init == 0) {
			//backUp = origin.clone();
			//init++; continue;
			//}
			if(!sethand) handDetect(0);
			else handTracking();
			imshow("Hough", origin);
			ch = waitKey(1);
			if (ch == 27) close();
		}
	}

	void handDetect(int flag) { //0 :ROI, 1: 전체영상
		int scale = 2;
		vector<vector<Point>> contours;
		Mat hierarchy;

		//	absdiff(origin, backUp, diff); //차영상 origin,backup은 3 channels이므로 diff도 3channels
		//	backUp = origin.clone();

		resize(origin, origin, sz);
		flip(origin, origin, 1); // y축 기준으로 뒤집기
		cvtColor(origin, hand, COLOR_RGB2YCrCb);
		inRange(hand, Scalar(0, 75, 130), Scalar(255, 133, 180), hand);
		//cvtColor(diff, diff, COLOR_RGB2GRAY);
		//imshow("hand", hand);
		//imshow("diff", diff);
		//hand = hand & diff;
		Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, hand.rows / 50));
		erode(hand, hand, verticalStructure);
		erode(hand, hand, verticalStructure);
		dilate(hand, hand, verticalStructure);
		dilate(hand, hand, verticalStructure);

		Mat center, dist_change;
		if (!flag) { //0
			imgROI = Mat(hand, ROI[0]);
			drawROI = Mat(origin, ROI[0]);
			center = Mat::zeros(Size(imgROI.rows, imgROI.cols), CV_8UC1);
			dist_change = Mat::zeros(Size(imgROI.rows, imgROI.cols), CV_8UC1);
			findContours(imgROI, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE, Point(0, 0));
			rectangle(origin, ROI[0], Scalar(255, 255, 0), 3); //draw ROI
		}
		else {
			center = Mat::zeros(Size(origin.rows, origin.cols), CV_8UC1);
			dist_change = Mat::zeros(Size(origin.rows, origin.cols), CV_8UC1);
			findContours(hand, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE, Point(0, 0));
		}

		int bigsize = 0;
		int index = 0;
		//calculate biggest contour
		if (contours.size() > 0) {
			for (int i = 0; i < contours.size(); i++)
			{
				double a = contourArea(contours[i], false);
				if (a > bigsize) {
					bigsize = a;
					index = i;
				}
			}
			if (index >= 0) {
				int j = 0;
				Mat cImg;
				if (!flag) {
					cImg = Mat(Size(imgROI.rows, imgROI.cols), CV_8UC1, Scalar(0));
					drawContours(drawROI, contours, index, Scalar(0, 0, 255), 3); //skin color contours
				}
				else {
					cImg = Mat(Size(hand.rows, hand.cols), CV_8UC1, Scalar(0));
					drawContours(origin, contours, index, Scalar(0, 0, 255), 3); //skin color contours
				}
				drawContours(center, contours, index, Scalar(255), -1);
				distanceTransform(center, dist_change, CV_DIST_L2, 5);
				int maxIdx[2];    //좌표 값을 얻어올 배열(행, 열 순으로 저장됨)
				double radius = 0;
				minMaxIdx(dist_change, NULL, &radius, NULL, maxIdx, center);   //최소값은 사용 X
				centerOfHand = Point(maxIdx[1], maxIdx[0]);

				if (centerOfHand.x > 0 && centerOfHand.y > 0 && centerOfHand.x < 800 && centerOfHand.y < 720) {
					circle(cImg, centerOfHand, radius*scale, Scalar(255), 3);
					cImg = cImg & center;
					imshow("cImg", cImg);

					contours.clear();
					findContours(cImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

					int fingercount = 0, thre = 15;
					for (int i = 0; i < contours.size(); i++)
						if (contours[i].size() > thre) fingercount++;
					//if (pre_fingercount - fingercount >= 4 || fingercount==1) putText(origin, "click event!!", Point(10, 45), 0, 1, Scalar(0, 0, 255),2);
					if (pre_fingercount - fingercount >= 4) {
						putText(origin, "click event!!", Point(10, 45), 0, 1, Scalar(0, 0, 255), 2);
						sethand = true;
					}
					str = "finger count : ";
					str += to_string(fingercount - 1);
					putText(origin, str, Point(10, 25), 0, 1, Scalar(0, 0, 255), 2);
					pre_fingercount = fingercount;
				}
				if (!flag) {
					circle(drawROI, centerOfHand, radius, Scalar(0, 0, 255), 3); //손바닥 빨간색
					circle(drawROI, centerOfHand, radius * scale, Scalar(255, 0, 255), 3); //손가락 기준되는 원 보라색
					circle(drawROI, centerOfHand, 10, Scalar(255, 255, 0), 3); //중심점 파란색
				}
				else {
					circle(origin, centerOfHand, radius, Scalar(0, 0, 255), 3); //손바닥 빨간색
					circle(origin, centerOfHand, radius * scale, Scalar(255, 0, 255), 3); //손가락 기준되는 원 보라색
					circle(origin, centerOfHand, 10, Scalar(255, 255, 0), 3); //중심점 파란색
				}
			}
		}
	}

	//Point pt_Measurement -> centerOfPoint
	//Mat state -> measurement
	void handTracking() {
		// Camera frame
		cv::Mat frame;

		// >>>> Kalman Filter
		int stateSize = 6;
		int measSize = 4;
		int contrSize = 0;

		unsigned int type = CV_32F;
		cv::KalmanFilter kf(stateSize, measSize, contrSize, type);

		cv::Mat state(stateSize, 1, type);  // [x,y,v_x,v_y,w,h]
		cv::Mat meas(measSize, 1, type);    // [z_x,z_y,z_w,z_h]
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
		kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
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

		//// Camera Capture
		//VideoCapture cap;
		//// >>>>> Camera Settings
		//if (!cap.open(0))
		//{
		//	//		cout << "Webcam not connected.\n" << "Please verify\n";
		//	return;
		//}
		//cap.set(CV_CAP_PROP_FRAME_WIDTH, 900);
		//cap.set(CV_CAP_PROP_FRAME_HEIGHT, 470);
		//// <<<<< Camera Settings

		char ch = 0;
		double ticks = 0;
		bool found = false;
		int notFoundCount = 0;

		// >>>>> Main loop
		while (ch != 27)
		{
			double precTick = ticks;
			ticks = (double)cv::getTickCount();

			double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds

			camera >> frame;
			flip(frame, frame, 1); // y축 기준으로 뒤집기
			Mat res;
			frame.copyTo(res);

			if (found)
			{
				// >>>> Matrix A
				kf.transitionMatrix.at<float>(2) = dT;
				kf.transitionMatrix.at<float>(9) = dT;
				// <<<< Matrix A

				cout << "dT:" << endl << dT << endl;

				state = kf.predict();
				cout << "State post:" << endl << state << endl;

				Rect predRect;
				predRect.width = state.at<float>(4);
				predRect.height = state.at<float>(5);
				predRect.x = state.at<float>(0) - predRect.width / 2;
				predRect.y = state.at<float>(1) - predRect.height / 2;

				Point center;
				center.x = state.at<float>(0);
				center.y = state.at<float>(1);
				circle(res, center, 2, CV_RGB(255, 0, 0), -1);
				rectangle(res, predRect, CV_RGB(255, 0, 0), 2);
				imshow("res", res);
			}

			// >>>>> preprocessing
			cvtColor(frame, res, COLOR_RGB2YCrCb);
			Mat rangeRes = Mat::zeros(frame.size(), CV_8UC1);
			inRange(res, Scalar(0, 75, 130), Scalar(255, 133, 180), rangeRes);
			erode(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
			dilate(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
			erode(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
			dilate(rangeRes, rangeRes, cv::Mat(), cv::Point(-1, -1), 2);
			// <<<<< preprocessing

			// Thresholding viewing
			cv::imshow("Threshold", rangeRes);

			// >>>>> Contours detection
			vector<vector<Point> > contours;
			findContours(rangeRes, contours, RETR_CCOMP, CV_CHAIN_APPROX_NONE);
			// <<<<< Contours detection
			double bigsize1 = 0, bigsize2 = 0;
			int index1=0, index2 = 0;
			vector<vector<Point> > hands;
			vector<Rect> handsBox;
			if (contours.size() > 0) {
				for (int i = 0; i < contours.size(); i++)
				{
					double a = contourArea(contours[i], false);
					if (a > bigsize2) {
						bigsize2 = a;
						index2 = i;
						if (bigsize2 > bigsize1) {
							bigsize1 = bigsize2;
							index1 = index2;
						}
					}
				}

				// >>>>> Filtering
			
				//			for (size_t i = 0; i < contours.size(); i++)
				//			{
				Rect hBox;
				hBox = boundingRect(contours[index1]); //2번째로 큰거

				float ratio = (float)hBox.width / (float)hBox.height;
				if (ratio > 1.0f)
					ratio = 1.0f / ratio;

				// Searching for a bBox almost square
	//			if (ratio > 0.75 && hBox.area() >= 400)
	//			{
					hands.push_back(contours[index1]);
					handsBox.push_back(hBox);
	//			}
				//			}
							// <<<<< Filtering

				cout << "Hands found:" << handsBox.size() << endl;

				// >>>>> Detection result
				//for (size_t i = 0; i < hands.size(); i++)
				//{

				drawContours(frame, contours, index1, CV_RGB(0, 0, 255), 2);
				rectangle(frame, handsBox[0], CV_RGB(0, 255, 0), 2);

				Point center;
				center.x = handsBox[0].x + handsBox[0].width / 2;
				center.y = handsBox[0].y + handsBox[0].height / 2;
				circle(frame, center, 2, CV_RGB(20, 150, 20), -1);

				stringstream sstr;
				sstr << "(" << center.x << "," << center.y << ")";
				putText(frame, sstr.str(),
					Point(center.x + 3, center.y - 3),
					FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(20, 150, 20), 2);
			}
//			}
			// <<<<< Detection result

			// >>>>> Kalman Update
			if (hands.size() == 0)
			{
				notFoundCount++;
				cout << "notFoundCount:" << notFoundCount << endl;
				if (notFoundCount >= 100)
				{
					found = false;
				}
				sethand = false;
				/*else
				kf.statePost = state;*/
			}
			else
			{
				notFoundCount = 0;

				meas.at<float>(0) = handsBox[0].x + handsBox[0].width / 2;
				meas.at<float>(1) = handsBox[0].y + handsBox[0].height / 2;
				meas.at<float>(2) = (float)handsBox[0].width;
				meas.at<float>(3) = (float)handsBox[0].height;

				if (!found) // First detection!
				{
					// >>>> Initialization
					kf.errorCovPre.at<float>(0) = 1; // px
					kf.errorCovPre.at<float>(7) = 1; // px
					kf.errorCovPre.at<float>(14) = 1;
					kf.errorCovPre.at<float>(21) = 1;
					kf.errorCovPre.at<float>(28) = 1; // px
					kf.errorCovPre.at<float>(35) = 1; // px

					state.at<float>(0) = meas.at<float>(0);
					state.at<float>(1) = meas.at<float>(1);
					state.at<float>(2) = 0;
					state.at<float>(3) = 0;
					state.at<float>(4) = meas.at<float>(2);
					state.at<float>(5) = meas.at<float>(3);
					// <<<< Initialization

					kf.statePost = state;

					found = true;
				}
				else
					kf.correct(meas); // Kalman Correction

									  //				cout << "Measure matrix:" << endl << meas << endl;
			}
			// <<<<< Kalman Update

			// Final result
			imshow("Tracking", frame);

			// User key
			ch = cv::waitKey(1);
		}
		// <<<<< Main loop
		close();
	}

	void close() {
		camera.release();
		cv::destroyAllWindows();
	}
};

int main(int argc, _TCHAR* argv[])
{
	Video video;
	namedWindow("Hough", CV_WINDOW_AUTOSIZE);
	video.readCam();
//	video.handTracking();
	return 0;
}