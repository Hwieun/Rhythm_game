#include "stdafx.h"
//#include <opencv2\opencv.hpp>
//#include <opencv2\imgproc.hpp>
//#include <opencv2\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <iostream>
#include <time.h>
#include <math.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>



using namespace cv;
using namespace std;

class Video {
private:
	VideoCapture camera;
	Mat origin, hand, labels, imgROI, drawROI;
	Size sz;
	float fps;
	float curTime;
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
		while (camera.read(origin))
		{
			if (ch == 27) close();
			curTime = clock();
			handDetect();
			curTime = (float)(clock() - curTime);
//			printf("curTime : %f\n", curTime);
			fps = (1 / curTime)*1000;
			printf("fps : %f\n", fps);
			ch = waitKey(1); //waitKey의 delay 단위는 ms
			imshow("Hough", origin);
		}
	}

	void handDetect() {
		vector<vector<Point>> contours;
		Mat hierarchy;
		Rect ROI(400, 220, 450, 400);

		resize(origin, origin, sz);
		flip(origin, origin, 1); // y축 기준으로 뒤집기
		cvtColor(origin, hand, COLOR_RGB2YCrCb);
		inRange(hand, Scalar(0, 77, 130), Scalar(255, 133, 180), hand);
		Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, hand.rows / 50));
		erode(hand, hand, verticalStructure);
		erode(hand, hand, verticalStructure);
		dilate(hand, hand, verticalStructure);
		dilate(hand, hand, verticalStructure);
		imgROI = Mat(hand, ROI);
		drawROI = Mat(origin, ROI);
		findContours(imgROI, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0,0));
		int bigsize = 0;
		int index = 0;

		rectangle(origin, Point(400, 220), Point(850, 620), Scalar(255, 255, 0), 3); //draw ROI

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
			vector<vector<Vec4i>> convexity(contours.size());
			vector<vector<int>> hullint(contours.size());
			vector<vector<Point>> hull(contours.size());
			for (int i = 0; i < contours.size(); i++) {
				convexHull(contours[i], hull[i], false); //false면 index, true면 point
				convexHull(contours[i], hullint[i], false); 
				if (hullint[i].size() > 3)
					convexityDefects(contours[i], hullint[i], convexity[i]);
			}
		
			int j = 0;
			double x = 0, y = 0;
			//int distance_thre = 200;
			if (index >= 0) {
				vector<Point> fingertips;
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
				
				/*for (int i = 0; i < hull.size(); i++) {
					int max = 0, maxindex = 0;
					for (j = 0; j < hull[i].size(); j++) {
						int bf = (int)sqrt(pow(centerOfHand.x - hull[i][j].x, 2) + pow(centerOfHand.y - hull[i][j].y, 2));
						if (max < bf) { max = bf; maxindex = j; }
					}
					fingertips.push_back(hull[i][maxindex]);
				}*/

				// Draw on origin	
//				drawContours(origin, contours, index, Scalar(0, 0, 255), 3); //skin color contours
				for (int i = 0; i < hull.size(); i++)
					drawContours(drawROI, hull, i, Scalar(0, 255, 255), 3); //convex hull
				for (int i = 0; i < contours.size(); i++)
				{
					for (const Vec4i& v : convexity[i]) {
						float depth = v[3] / 256;
						if (depth > 15) {
							int startidx = v[0]; Point ptstart(contours[i][startidx]);
							int endidx = v[1]; Point ptend(contours[i][endidx]);
							int faridx = v[2]; Point ptfar(contours[i][faridx]);

					//		line(drawROI, ptstart, ptend, Scalar(255, 0, 255), 1);
					//		line(origin, ptstart, ptfar, Scalar(255, 0, 255), 1);
					//		line(origin, ptfar, ptend, Scalar(255, 0, 255), 1);
					//		circle(origin, ptfar, 3, Scalar(0, 0, 255), 2);
							fingertips.push_back(ptstart);
							fingertips.push_back(ptend);
						}
					}
				}
				if (centerOfHand.x > 0 && centerOfHand.y > 0 && centerOfHand.x < 250 && centerOfHand.y < 300)
					circle(drawROI, centerOfHand, 10, Scalar(255, 255, 0), 3);
				for (int i = 0; i < fingertips.size(); i++)
					circle(drawROI, fingertips[i], 10, Scalar(0, 0, 255), 3);
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