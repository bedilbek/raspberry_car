// LaneDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "binarization_utils.h"
#include "bird_eye.h"
#include <vector>

using namespace std;
using namespace cv;

int x = 100, y = 80;
int h = 0, s = 40, v = 102;
int hm = 5, sm = 255, vm = 255;

Scalar min_th = { 0, 0, 0 };
Scalar max_th = { 5, 5, 255 };

void update(int, void*)
{
	min_th = Scalar(h, s, v);
	max_th = Scalar(hm, sm, vm);
}

bool is_stop_detected(Mat frame)
{
	int scale = 4;
	Mat kernel = Mat(3, 3, CV_8UC1, Scalar(1));
	Mat img;
	resize(frame, img, Size(frame.cols / scale, frame.rows / scale));
	Mat3b bgr_inv = ~img;
	Mat3b hsv_inv;
	cvtColor(bgr_inv, hsv_inv, COLOR_BGR2HSV);
	Mat1b mask;
	inRange(hsv_inv, Scalar(90 - 15, 30, 230), Scalar(90 + 15, 255, 255), mask); // Cyan is 90
	erode(mask, mask, kernel);
	vector<Point> points;
	findNonZero(mask, points);
	if (points.size() > 60)
	{
		Point2f center;
		float radius;
		minEnclosingCircle(points, center, radius);
		if (radius < (55 / scale) && radius > (50 / scale) && points.size() / (3.14*radius*radius) > 0.70)
			return true;
	}
	return false;
}

int main()
{
	namedWindow("Controls");
	createTrackbar("x", "Controls", &x, 255);
	createTrackbar("y", "Controls", &y, 255);
	createTrackbar("H", "Controls", &h, 230, update);
	createTrackbar("S", "Controls", &s, 230, update);
	createTrackbar("V", "Controls", &v, 230, update);
	//createTrackbar("Hm", "Controls", &hm, 255, update);
	//createTrackbar("Sm", "Controls", &sm, 255, update);
	//createTrackbar("Vm", "Controls", &vm, 255, update);


	Mat img = imread("right2.jpg");
	Mat kernel = Mat(5, 5, CV_8UC1, Scalar(1));

	VideoCapture video;
	video.open("sign_sample.avi");
	int thresh = 15;
	bool playback = true;
	while (true)
	{
		//if(playback)
		//	video >> img;
		//if (img.empty())
		//	break;

		Mat3b bgr_inv = ~img;
		Mat3b hsv_inv;
		cvtColor(bgr_inv, hsv_inv, COLOR_BGR2HSV);
		Mat1b mask;
		inRange(hsv_inv, Scalar(0 - 15, 6, 150), Scalar(0 + 15, 255, 255), mask); // Cyan is 90
		//imshow("Before", mask);
		erode(mask,mask,kernel);
		
		vector<Point> points;
		findNonZero(mask, points);
		if (points.size() > 60)
		{
			Point2f center;
			float radius;
			minEnclosingCircle(points, center, radius);
			//cout << "R: " << radius << " A: " << points.size() / (3.14*radius*radius) << endl;
			if (radius < 55 && radius > 32 && points.size() / (3.14*radius*radius) > 0.5)
			{
				circle(mask, center, radius, 150);
				vector<int> sum;
				reduce(mask, sum, 0, REDUCE_SUM);
				double min, max;
				int min_idx, max_idx;
				minMaxIdx(sum, &min, &max, &min_idx, &max_idx);
				if (min_idx > max_idx)
				{
					cout << "Right" << endl;
				}
				else
				{
					cout << "Left" << endl;
				}
			}
		}

		imshow("After", mask);
		imshow("org", bgr_inv);

		if (waitKey(10) == 'p')
			playback = !playback;
	}
	return 0;
}