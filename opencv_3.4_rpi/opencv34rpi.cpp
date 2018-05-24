#include "stdafx.h"
#include <opencv2\core\core.hpp>
#include <iostream>
#include <time.h>
#include <math.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\mat.hpp>


using namespace cv;
using namespace std;

class Video {
private:
	VideoCapture camera;
	Mat origin, hand, labels, imgROI, drawROI, backUp;
	Size sz;
	float fps;
	float curTime; int kk = 1;
public:
	Video() {
		sz.width = 1280;
		sz.height = 720;
		camera = VideoCapture(0);
		if (!camera.isOpened())
			printf("Camera not connected!");
	}

	void readCam() {
		char ch = 0;
		char init = 0;
		while (camera.read(origin))
		{
			if (ch == 27) close();
			curTime = clock();
			if (init == 0) {
				backUp = origin.clone();
//				resize(backUp, backUp, sz);
				hand = origin.clone();
				init++; continue; }
			handDetect();
			curTime = (float)(clock() - curTime);
//			printf("curTime : %f\n", curTime);
			fps = (1 / curTime)*1000;
			printf("fps : %f\n", fps);
			ch = waitKey(1); //waitKey�� delay ������ ms
			imshow("Hough", origin);
			
		}
	}

	void handDetect() {
		vector<vector<Point>> contours;
		Mat hierarchy, bf;
		Rect ROI(400, 220, 450, 400);
		absdiff(origin, backUp, hand);
		imshow("hand", hand);
//		if (kk == 2)
	//	{
			backUp = origin.clone();
	//		kk = 0;
	//	}
		resize(origin, origin, sz);
//		resize(hand, hand, sz);
		flip(origin, origin, 1); // y�� �������� ������
		cvtColor(origin, hand, COLOR_RGB2YCrCb);
		inRange(hand, Scalar(5, 77, 130), Scalar(250, 133, 180), hand);
		Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, hand.rows / 50));
 		erode(hand, hand, verticalStructure);
		erode(hand, hand, verticalStructure);
		dilate(hand, hand, verticalStructure);
		dilate(hand, hand, verticalStructure);

		kk++;
		
		imgROI = Mat(hand, ROI);
		drawROI = Mat(origin, ROI);
		findContours(hand, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0,0));
		int bigsize = 0;
		int index = 0;
//		rectangle(origin, Point(400, 220), Point(850, 620), Scalar(255, 255, 0), 3); //draw ROI

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
			if (index >= 0){
				vector<vector<Vec4i>> convexity(contours.size());
				vector<vector<int>> hullint(contours.size());
				vector<vector<Point>> hull(contours.size());
				for (int i = 0; i < contours.size(); i++) {
					convexHull(contours[index], hull[i], false); //false�� index, true�� point
					convexHull(contours[i], hullint[i], false);
					if (hullint[i].size() > 3)
						convexityDefects(contours[i], hullint[i], convexity[i]);
				}
				
				int j = 0;
				double x = 0, y = 0;
				//int distance_thre = 200;

					// calculate center point of biggest contour
				for (int i = 0; i < hull.size(); i++) {
					for (j = 0; j < hull[i].size(); j++) {
						x = x + hull[i][j].x;
						y = y + hull[i][j].y;
					}
				}
				--j;
				int bf = hull.size()*j;
				printf("%d\n", hull.size());
				Point centerOfHand = Point(x / bf, y / bf);

				// Draw on origin	
//				drawContours(origin, contours, index, Scalar(0, 0, 255), 3); //skin color contours
				drawContours(origin, hull, index, Scalar(0, 255, 255), 3); //convex hull

				int mindistance = 720;
				for (int i = 0; i < contours.size(); i++)
				{
					for (const Vec4i& v : convexity[i]) {
						float depth = v[3] / 256;
						if (depth > 15) {
							//int startidx = v[0]; Point ptstart(contours[i][startidx]);
							//int endidx = v[1]; Point ptend(contours[i][endidx]);
							int faridx = v[2]; Point ptfar(contours[i][faridx]);

							int n = sqrt(pow(ptfar.x - centerOfHand.x, 2) + pow(ptfar.y - centerOfHand.y, 2));
							if (mindistance < n) mindistance = n;
						}
					}
				}

				//Mat sub = hand - andimg; //�հ��� ���ϱ�
				//imshow("finger", sub);
				//andimg.zeros(hand.rows, hand.cols, hand.type()); //�ʱ�ȭ�ؼ� ����
				//circle(andimg, centerOfHand, mindistance, Scalar(1)); //��
				//imshow("circle", andimg);
				//Mat merge = sub + andimg; //5��° ����
				//imshow("merge finger and circle", merge);
				////convex hull
				//andimg.zeros(hand.rows, hand.cols, hand.type()); //�ʱ�ȭ�ؼ� ����
				//for (int i = 0; i < hull.size(); i++)
				//	fillConvexPoly(andimg, hull[i], Scalar(255, 255, 0));
				//imshow("convex hull", andimg);

				//and ����

	//			if (centerOfHand.x > 0 && centerOfHand.y > 0 && centerOfHand.x < 850 && centerOfHand.y < 620)
					circle(drawROI, centerOfHand, 10, Scalar(255, 255, 0), 3);
			}
		}
		
	}

	void close() {
		camera.release();
		cv::destroyAllWindows();
	}
};

int main() {
	Video video;
	int thres = 95, num = 0;
	namedWindow("Hough", CV_WINDOW_AUTOSIZE);

	video.readCam();

	return 0;
}