// LaneDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "binarization_utils.h"
#include "bird_eye.h"
#include <vector>

using namespace std;

int thr = 0, str = 0;
int h = 0, s = 40, v = 102;
int hm = 5, sm = 255, vm = 255;
int center = 117;
int clip_limit = 2;
int tile_grid_size = 8;

int b = 2;
int c = 20;

Scalar min_th = { 18, 52, 102 };
Scalar max_th = { 255, 255, 255 };

void update(int, void*)
{
	min_th = Scalar(h, s, v);
	max_th = Scalar(hm, sm, vm);
}

double minth = 0;
double maxth = 0;

int main()
{
	int roi = 20;
	bool playback = true;
	Mat frame, mask;
	Mat fr, bc;
	int indxl = -1, indxr = -1;
	int pindxl = -1, pindxr = -1;
	vector<int> reduced;
	int seq = -1;
	int threshold = 50;
	int interval = 3;
	int pthreshold = 3;
	

	namedWindow("Controls", WINDOW_GUI_NORMAL);
	createTrackbar("H", "Controls", &h, 255, update);
	createTrackbar("S", "Controls", &s, 255, update);
	createTrackbar("V", "Controls", &v, 255, update);

	createTrackbar("Hm", "Controls", &hm, 255, update);
	createTrackbar("Sm", "Controls", &sm, 255, update);
	createTrackbar("Vm", "Controls", &vm, 255, update);

	createTrackbar("Clip", "Controls", &clip_limit, 100);
	createTrackbar("Tile", "Controls", &tile_grid_size, 100);

	createTrackbar("B", "Controls", &b, 100);
	createTrackbar("C", "Controls", &c, 100);

	VideoCapture video("output.avi");
	
	
	int offset = 0;
	int prev_offset = 0;
	int direction = 0;
	int frame_count = 0;
	int lane_width = 0;

	while (1)
	{
		if (playback)
		{
			video >> frame;
			//Yellow Mask
			if (frame.empty())
				break;
		}

		frame_count++;
		
		Mat colorMask = Mat(frame.rows, frame.cols, CV_8UC1, Scalar(0));
		cvtColor(frame, colorMask, COLOR_BGR2GRAY);
		
		//Bird eye transformation
		colorMask = bird_eye(colorMask, fr, bc, false);
		resize(colorMask, colorMask, Size(colorMask.cols / 5, colorMask.rows / 5));
		Mat original;
		resize(frame, original, Size(colorMask.cols, colorMask.cols * frame.rows / frame.cols));

		GaussianBlur(colorMask, colorMask, Size(3, 3), 20);
		Canny(colorMask, colorMask, 0, 40);
		colorMask = colorMask(Range(colorMask.rows * 0.8 , colorMask.rows - 40), Range::all());
		floodFill(colorMask, Point(colorMask.cols / 2 + prev_offset, colorMask.rows - 5), 150);
		inRange(colorMask, { 149 }, { 151 }, colorMask);

		int p_offset = 5;
		Point pl1 = Point(0, 0);
		Point pl2 = Point(0, 0);

		for (int j = 0, c = 0; j < colorMask.cols; j++)
		{
			if (colorMask.at<unsigned char>(colorMask.rows - 2, j) > 0)
			{
				pl1 = Point(j, colorMask.rows - 2);
				break;
			}
		}

		for (int j = 0, c = 0; j < colorMask.cols; j++)
		{
			if (colorMask.at<unsigned char>(p_offset, j) > 0)
			{
				pl2 = Point(j, p_offset);
				break;
			}
		}

		Point pr1 = Point(0, 0);
		Point pr2 = Point(0, 0);
		Mat flipped;

		colorMask.copyTo(flipped);
		cv::flip(flipped, flipped, 1);

		for (int j = 0, c = 0; j < flipped.cols; j++)
		{
			if (flipped.at<unsigned char>(flipped.rows - 2, j) > 0)
			{
				pr1 = Point(j, flipped.rows - 2);
				break;
			}
		}

		for (int j = 0, c = 0; j < flipped.cols; j++)
		{
			if (flipped.at<unsigned char>(p_offset, j) > 0)
			{
				pr2 = Point(j, p_offset);
				break;
			}
		}

		double angl = 1.57;
		double angr = 1.57;
		
		if ((pl1.x - pl2.x) != 0)
			angl = atan(((double)(pl2.y - pl1.y) / (double)(pl2.x - pl1.x)));

		if ((pr1.x - pr2.x) != 0)
			angr = atan(((double)(pr2.y - pr1.y) / (double)(pr2.x - pr1.x)));

		if (abs(angl) > abs(angr) && angl > 0)
			direction = -1;
		else if ( angr > 0)
			direction = 1;
		else
			direction = 0;

		prev_offset = 50 * direction;

		//if (playback)
		//	cout << (direction == 0 ? "forward" : direction == -1 ? "left" : "right") << "  " << (direction == 0 ? 90 : (direction == -1 ? 180 * angr / 3.14 : 180 * angl / 3.14)) << endl;

		if (frame_count < 5)
		{
			int l = 0, r = 0;
			vector<int> lane;
			reduce(colorMask, lane, 0, REDUCE_SUM);

			for (l = 0; l < lane.size() / 2; l++)
				if (lane[l] > 0)
					break;

			for (int i = lane.size() - 1; i > 0; r++, i --)
				if (lane[i] > 0)
					break;

			lane_width += lane.size() - l - r;
			if (frame_count == 4)
				lane_width /= frame_count;
		}
		else
		{
			int nonzero = 0;
			if (direction == 0)
			{
				for (nonzero = 0; nonzero < colorMask.cols; nonzero++)
					if (colorMask.at<unsigned char>(colorMask.rows - 2, nonzero) > 0)
						break;
				offset = colorMask.cols / 2 - lane_width / 2 - nonzero;
			}
			else if (direction == 1)
			{
				if ((pl1.x - pl2.x) != 0)
					angr = atan(abs((double)(pl2.y - pl1.y) / (double)(pl2.x - pl1.x)));
				offset = colorMask.cols / 2 - pl1.x - lane_width / 2 * sin(angr);
				offset *= -1;
			}
			else
			{
				if ((pr1.x - pr2.x) != 0)
					angl = atan(abs((double)(pr2.y - pr1.y) / (double)(pr2.x - pr1.x)));
				offset = colorMask.cols / 2 - pr1.x - lane_width / 2 * sin(angl);
			}

			line(original, Point(original.cols / 2 + offset, original.rows - 5), Point(original.cols / 2, original.rows - 5), 150, 5);

			if (playback)
				cout << offset << endl;
		}

		imshow("R", original);

		if (waitKey(150) == 'p')
			playback = !playback;
	}
	std::cout << "finished" << endl;
	return 0;
}