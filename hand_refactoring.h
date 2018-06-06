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
	// @origin : ����ڿ��� ������ ����
	// @hand : ����ó���� matrix, ����ȭ�� �տ���, origin�� ���� ������
	// @handROI : hand�� �Ϻο���, ������ ����ó���� �Ǵ� ����
	// @originROI : oringin�� �Ϻο���, ����ó���� �� ����� �׷����� ����. handROI ���� ������.
	Mat origin, hand, handROI, originROI;
	// @diff : origin�̶� backup�̶� ����.
	// @backup : origin���� ����(�̹���).
	Mat diff, backup;
	// @preFingerCount : �����հ��� ����.
	// @largestIndex : hand contour�� �� ���� ū contour�� index
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