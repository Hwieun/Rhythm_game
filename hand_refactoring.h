#ifndef CAP
#define CAP
#include "stdafx.h"
#endif

#define YCbCr
//#define HSV
#define Detecting 1
#define Tracking 2

#define VIDEO1 "1_banpal_hanson_color.avi"
#define VIDEO2 "2_banpal_hanson_click_tracking.avi"
#define VIDEO3 "3_banpal_dooson_click_tracking_hide.avi"
#define VIDEO4 "4_ginpal_dooson_color.avi"
#define VIDEO5 "5_ginpal_dooson_click_tracking_hide.avi"


class Cap {
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
	Mat& Cap::ReadCam();
	void HandDetect();
	void HandTracking();
	void Drawing(int level, int flag);
	Point GetPoint();
	void Close();
};