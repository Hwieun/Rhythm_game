#include "stdafx.h"
#include "hand_refactoring.h"

#define Detecting 1
#define Tracking 2
#define VIDEO1 "1_banpal_hanson_color.avi"
#define VIDEO2 "2_banpal_hanson_click_tracking.avi"
#define VIDEO3 "3_banpal_dooson_click_tracking_hide.avi"
#define VIDEO4 "4_ginpal_dooson_color.avi"
#define VIDEO5 "5_ginpal_dooson_click_tracking_hide.avi"

void Video::ReadCam() {
	while (camera.read(origin))
	{
		if (!isClicked) {

			HandDetect();

			if (contours.size() > 0)
				Drawing(2, Detecting);
		}
		else {
			HandTracking();

		}

		imshow("main", origin);
		if (27 == waitKey(1)) Close();
	}
}

void Video::HandDetect() {
	resize(origin, origin, sz);
	flip(origin, origin, 1);
	cvtColor(origin, hand, COLOR_RGB2YCrCb);

#ifdef TRACK_BAR
#ifdef YCbCr
	tb[0][0] = getTrackbarPos("y_min", "YCbCr");
	tb[0][1] = getTrackbarPos("y_max", "YCbCr");

	tb[1][0] = getTrackbarPos("cb_min", "YCbCr");
	tb[1][1] = getTrackbarPos("cb_max", "YCbCr");

	tb[2][0] = getTrackbarPos("cr_min", "YCbCr");
	tb[2][1] = getTrackbarPos("cr_max", "YCbCr");

	inRange(cuda_cvtColor_ycrcb, Scalar(tb[0][0], tb[1][0], tb[2][0]), Scalar(tb[0][1], tb[1][1], tb[2][1]), cuda_inrange);
	imshow("cuda_inrange_out_ycrcb", cuda_inrange);
#endif
#ifdef HSV
	tb[0][0] = getTrackbarPos("h_min", "HSV");
	tb[0][1] = getTrackbarPos("h_max", "HSV");

	tb[1][0] = getTrackbarPos("s_min", "HSV");
	tb[1][1] = getTrackbarPos("s_max", "HSV");

	tb[2][0] = getTrackbarPos("v_min", "HSV");
	tb[2][1] = getTrackbarPos("v_max", "HSV");

	cuda_inRange(cuda_cvtColor_hsv, Scalar(tb[0][0], tb[1][0], tb[2][0]), Scalar(tb[0][1], tb[1][1], tb[2][1]), cuda_inrange);
	imshow("cuda_inrange_out_hsv", cuda_inrange);
#endif
#else
#ifdef YCbCr
	inRange(hand, Scalar(30, 75, 130), Scalar(255, 133, 180), hand);
	imshow("ycbcr", hand);
#endif
#ifdef HSV
	cuda_inRange(cuda_cvtColor_hsv, Scalar(0, 28, 115), Scalar(16, 122, 253), cuda_inrange);
	imshow("cuda_inrange_hsv", cuda_inrange);
#endif
#endif


	//#Q1.kernel size
	//#Q2.morphology sequence
	Mat verticalStructure = getStructuringElement(MORPH_ELLIPSE, Size(4, 4));
	//imshow("before", hand);
	erode(hand, hand, verticalStructure);
	verticalStructure = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	erode(hand, hand, verticalStructure);
	imshow("morphology", hand);

	Mat center = Mat::zeros(Size(ROI[0].width, ROI[0].height), CV_8UC1);
	Mat distChange = center.clone();
	handROI = Mat(hand, ROI[0]);
	originROI = Mat(origin, ROI[0]);
	Mat hierarchy;
	findContours(handROI, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE, Point(0, 0));
	rectangle(origin, ROI[0], Scalar(255, 255, 0), 3); //draw ROI

	int bigsize = 0;
	//calculate biggest contour
	if (contours.size() > 0) {
		for (int i = 0; i < contours.size(); i++) {
			double a = contourArea(contours[i], false);
			if (a > bigsize) {
				bigsize = a;
				largestIndex = i;
			}
		}
		if (largestIndex >= 0) { //중심점, 손가락
			int j = 0;
			Mat cImg;
			//if (!flag) 
			cImg = Mat(Size(handROI.rows, handROI.cols), CV_8UC1, Scalar(0));
			//else 
			//   cImg = Mat(Size(hand.rows, hand.cols), CV_8UC1, Scalar(0));
			drawContours(center, contours, largestIndex, Scalar(255), -1);
			distanceTransform(center, distChange, CV_DIST_L2, 5);
			int maxIdx[2];    //좌표 값을 얻어올 배열(행, 열 순으로 저장됨)
			minMaxIdx(distChange, NULL, &radius, NULL, maxIdx, center);   //최소값은 사용 X
			centerOfHand = Point(maxIdx[1], maxIdx[0]);

			if (centerOfHand.x > 0 && centerOfHand.y > 0 && centerOfHand.x < 800 && centerOfHand.y < 720) {
				str.clear();
				str += "radius : ";
				str += to_string(radius);
				putText(origin, str, Point(10, 75), 0, 1, Scalar(0, 0, 255), 2);
				scale = 1.8;
				circle(cImg, centerOfHand, radius*scale, Scalar(255), 3);
				cImg = cImg & center;
				//imshow("cImg", cImg);
				findContours(cImg, circleContour, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

				int fingercount = 0, thre = 5;
				for (int i = 0; i < circleContour.size(); i++)
					if (circleContour[i].size() > thre) fingercount++;
				if (preFingerCount - fingercount >= 4) {
					putText(origin, "click event!!", Point(10, 45), 0, 1, Scalar(0, 0, 255), 2);
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
	//HandDetect();
	////bounding 코드 추가(측정값 드로우)
	//if (){ // bounding에서 찾은 object가 없으면
	//   notFoundCount++;
	//   cout << "notFoundCount:" << notFoundCount << endl;
	//   if (notFoundCount >= 100){
	//      notFoundCount = 0;
	//      foundHand = false;
	//      isClicked = false;
	//      ROI[0].x = 700, ROI[0].y = 120;
	//      return;
	//   }
	//}
	//if (!foundHand){
	//   //2.D.초기화
	//   foundHand = true;
	//   kf.predict();
	//}
	//else{
	//   kf.correct();
	//   kf.predict();
	//}

	//Drawing();
	double ticks = 0;

	ticks = (double)cv::getTickCount();
	double dT = (ticks - precTick) / getTickFrequency(); //seconds
	precTick = ticks;
	HandDetect();

	//      vector<vector<Point>> hands;
	vector<Rect> handsBox;
	if (contours.size() > 0) {
		Rect hBox = boundingRect(contours[largestIndex]);

		//float ratio = (float)hBox.width / (float)hBox.height;
		//if (ratio > 1.0f)
		//   ratio = 1.0f / ratio;

		//         hands.push_back(contours[largestIndex]);
		handsBox.push_back(hBox);

		drawContours(originROI, contours, largestIndex, CV_RGB(0, 0, 255), 2);
		rectangle(originROI, handsBox[0], CV_RGB(0, 255, 0), 2);
		circle(originROI, centerOfHand, 2, CV_RGB(20, 150, 20), -1);
		printf("center point :(%d, %d)\nROI :(%d, %d)\n", centerOfHand.x, centerOfHand.y, ROI[0].x, ROI[0].y);

		stringstream sstr;
		sstr << "(" << centerOfHand.x << "," << centerOfHand.y << ")";
		putText(originROI, sstr.str(), Point(centerOfHand.x + 3, centerOfHand.y - 3), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(20, 150, 20), 2);
	}
	// <<<<< Detection result Drawing

	if (handsBox.size() == 0) {
		notFoundCount++;
		cout << "notFoundCount:" << notFoundCount << endl;
		if (notFoundCount >= 100) {
			notFoundCount = 0;
			foundHand = false;
			isClicked = false;
			ROI[0].x = 700, ROI[0].y = 120;
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

		//         state = kf.predict();
	}
	else {
		if (notFoundCount == 0) {
			cout << "State Measure:" << endl << measure << endl;
			kf.correct(measure);
		}
		kf.transitionMatrix.at<float>(2) = dT;
		kf.transitionMatrix.at<float>(9) = dT;
		printf("dT :%lf\n", dT);
		state = kf.predict();
		cout << "State post:" << endl << state << endl;
	}

	ROI[0].width = state.at<float>(4);
	ROI[0].height = state.at<float>(5);

	ROI[0].x = state.at<float>(0) - ROI[0].width / 2;
	if (ROI[0].x < 0) ROI[0].x = 0;
	else if (ROI[0].x + ROI[0].width > sz.width) ROI[0].width = sz.width - ROI[0].x;
	else if (ROI[0].x > sz.width || ROI[0].width < 0) {
		ROI[0].x = 700; ROI[0].width = 500;
	}

	ROI[0].y = state.at<float>(1) - ROI[0].height / 2;
	if (ROI[0].y < 0) ROI[0].y = 0;
	else if (ROI[0].y + ROI[0].height > sz.height) ROI[0].height = sz.height - ROI[0].y;
	else if (ROI[0].x > sz.width || ROI[0].width < 0) {
		ROI[0].y = 120; ROI[0].height = 500;
	}
	//if (ROI[0].x < 0) ROI[0].x = 0;
	//else if (ROI[0].x + ROI[0].width > sz.width) ROI[0].width = (sz.width - state.at<float>(0)) * 2;
	//if (state.at<float>(1) - ROI[0].height / 2 < 0) ROI[0].y = 0;
	//else if (state.at<float>(1) + ROI[0].height / 2 > sz.height) ROI[0].height = (sz.height - state.at<float>(1)) * 2;
	//>>>>>ROI 재설정

	/*Drawing(3);*/ ///predict 결과값 그리기
	drawContours(originROI, contours, largestIndex, CV_RGB(0, 255, 0), 2);
	rectangle(originROI, handsBox[0], CV_RGB(0, 255, 255), 2);
	circle(originROI, centerOfHand, 2, CV_RGB(0, 255, 255), -1);
}

void Video::Drawing(int level, int flag) {
	// contours centerOfHand radius circleContour
	//contour만
	if (level >= 1) {
		drawContours(originROI, contours, largestIndex, Scalar(0, 0, 255), 3); //skin color contours
	}
	//원3개. 중심점,손바닥원,손가락원
	if (level >= 2) {
		circle(originROI, centerOfHand, 10, Scalar(255, 255, 0), 3); //중심점 파란색
		circle(originROI, centerOfHand, radius, Scalar(0, 0, 255), 3); //손바닥 빨간색
		circle(originROI, centerOfHand, radius * scale, Scalar(255, 0, 255), 3); //손가락 기준되는 원 보라색

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

#ifdef TRACK_BAR
void Video::MakeTrackBar() {

}

void Video::GetTrackBar() {
#ifdef YCbCr
	tb[0][0] = getTrackbarPos("y_min", "YCbCr");
	tb[0][1] = getTrackbarPos("y_max", "YCbCr");

	tb[1][0] = getTrackbarPos("cb_min", "YCbCr");
	tb[1][1] = getTrackbarPos("cb_max", "YCbCr");

	tb[2][0] = getTrackbarPos("cr_min", "YCbCr");
	tb[2][1] = getTrackbarPos("cr_max", "YCbCr");

	inRange(hand, Scalar(tb[0][0], tb[1][0], tb[2][0]), Scalar(tb[0][1], tb[1][1], tb[2][1]), hand);
	imshow("ycbcr", origin);
#endif
#ifdef HSV
	tb[0][0] = getTrackbarPos("h_min", "HSV");
	tb[0][1] = getTrackbarPos("h_max", "HSV");

	tb[1][0] = getTrackbarPos("s_min", "HSV");
	tb[1][1] = getTrackbarPos("s_max", "HSV");

	tb[2][0] = getTrackbarPos("v_min", "HSV");
	tb[2][1] = getTrackbarPos("v_max", "HSV");

	inRange(hand, Scalar(tb[0][0], tb[1][0], tb[2][0]), Scalar(tb[0][1], tb[1][1], tb[2][1]), hand);
	imshow("inrange_out_hsv", origin);
#endif
}
#endif




#include "hand_refactoring.h"

int main(int argc, char *argv[])
{
	Video video;
	int thres = 95, num = 0;
	namedWindow("main", CV_WINDOW_AUTOSIZE);
	moveWindow("main", 0, 0);
	video.ReadCam();
	return 0;

	//QApplication a(argc, argv);
	//Rhythm_Game w;
	//w.show();
	//return a.exec();
}