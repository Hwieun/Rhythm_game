#include "stdafx.h"
#include <opencv2\opencv.hpp>
//#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <iostream>

#define PI 3.1415926

using namespace cv;

int main() {
	VideoCapture video("C:\\Users\\Kim Hwieun\\Desktop\\kkk\\test.mp4");
	Mat img, bfimg;
//	Mat contours;
	if (!video.isOpened()) return 0;

	int thres = 85;
	std::vector<Vec4i> lines;

//	img = imread("C:\\Users\\Kim Hwieun\\Desktop\\kkk\\cap.PNG", IMREAD_COLOR);
	Mat houghImg = img.clone();

	cvtColor(img, img, COLOR_BGR2GRAY);		
	GaussianBlur(img, img, Size(3, 3), 0, 0, BORDER_DEFAULT);
	Canny(img, img, thres, thres*2, 3);
//	Sobel(img, bfimg, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
//	convertScaleAbs(bfimg, img);
	HoughLinesP(img, lines, 1, CV_PI / 120, 50,0,0);

	thres = 1;
	lines.clear();
	HoughLinesP(img, lines, 1, CV_PI / 180, 70, 0, 0);
	for (size_t i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		if (l[1] / thres != l[3] / thres) //가로 줄 걸러냄
			line(houghImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
//			rectangle(houghImg, rec, Scalar(0, 0, 255));
	}

	namedWindow("Hough", CV_WINDOW_AUTOSIZE);
	imshow("Hough", houghImg);

//	video.release();	
	waitKey(0);
	cv::destroyAllWindows();
	return 0;
}