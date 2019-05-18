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

int b = 50;
int c = 100;

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
	//createTrackbar("V", "Controls", &v, 255, update);

	//createTrackbar("Hm", "Controls", &hm, 255, update);
	//createTrackbar("Sm", "Controls", &sm, 255, update);
	//createTrackbar("Vm", "Controls", &vm, 255, update);

	//createTrackbar("Clip", "Controls", &clip_limit, 100);
	//createTrackbar("Tile", "Controls", &tile_grid_size, 100);

	//createTrackbar("B", "Controls", &b, 100);
	//createTrackbar("C", "Controls", &c, 100);

	VideoCapture video("output.mp4");
	
	
	int offset = 0;
	int prev_offset = 0;
	int direction = 0;
	int frame_count = 0;
	int lane_width = 165;

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


		Mat colorMask = frame;//Mat(frame.rows, frame.cols, CV_8UC1, Scalar(0));

		resize(colorMask, colorMask, Size(colorMask.cols / 3, colorMask.rows / 3));
		
		
		//Bird eye transformation
		colorMask = bird_eye(colorMask, fr, bc, 3, 50);

		cvtColor(colorMask, colorMask, COLOR_BGR2GRAY);
		Mat white_mask;
		inRange(colorMask,  Scalar(185), Scalar(255), white_mask);
		//imshow("white", white_mask);
		
		GaussianBlur(colorMask, colorMask, Size(3, 3), tile_grid_size);
		Canny(colorMask, colorMask, b, c);
		colorMask = colorMask(Range(colorMask.rows * 0.8, colorMask.rows - 20), Range::all());
		white_mask = white_mask(Range(white_mask.rows * 0.8, white_mask.rows - 20), Range::all());
		imshow("white", white_mask);

		int resize_factor = 3;
		resize(white_mask, white_mask, Size(white_mask.cols / resize_factor, white_mask.rows/ resize_factor));
		int h_segment = 3, w_segment = 4;
		for (int i = 0; i < h_segment; i++)
		{
			for (int j = 0; j < w_segment; j++)
			{
				int seg_x1 = j * white_mask.cols / w_segment;
				int seg_x2 = (j + 1) * white_mask.cols / w_segment;
				int seg_y1 = i * white_mask.rows / h_segment;
				int seg_y2 = (i + 1 ) * white_mask.rows / h_segment;

				Mat white_area = white_mask(Range(seg_y1, seg_y2), Range(seg_x1, seg_x2));
				Mat nonzero;
				findNonZero(white_area, nonzero);
				if ((double)nonzero.rows / (double)(white_mask.cols / w_segment * white_mask.cols / w_segment) > 0.0)
				{
					vector<double> line_params;
					fitLine(nonzero, line_params, CV_DIST_HUBER, 0, 0.01, 0.01);
					Point p1, p2;

					p1 = Point((seg_x1 + 1000 * line_params[0] + line_params[2]) * resize_factor, (seg_y1 + 100 * line_params[1] + line_params[3]) * 3);
					p2 = Point((seg_x1 + -1000 * line_params[0] + line_params[2]) * resize_factor, (seg_y1 + -100 * line_params[1] + line_params[3]) * 3);

					//line(colorMask, p1, p2, 150, 10);
					//rectangle(white_mask, Point(seg_x1, seg_y1), Point(seg_x2, seg_y2), 180);
				}
				//imshow("s", white_area);
				//waitKey(500);
			}
		}
		imshow("seg", colorMask);

		
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
		bool left_lane = true, right_lane = true;

		if ((pl1.x - pl2.x) != 0)
			angl = atan(((double)(pl2.y - pl1.y) / (double)(pl2.x - pl1.x)));
		else if (pl1.x < 2)
			left_lane = false;

		if ((pr1.x - pr2.x) != 0)
			angr = atan(((double)(pr2.y - pr1.y) / (double)(pr2.x - pr1.x)));
		else if (pr1.x < 2)
			right_lane = false;

		if (left_lane && !right_lane && abs(angl) < 1.52)
			direction = 1;
		else if (!left_lane && right_lane && abs(angr) < 1.52)
			direction = -1;
		else if (left_lane && right_lane && abs(angr) < 1.52 && abs(angl) < 1.52)
		{
			if (angl > 0 && angr < 0)
				direction = -1;
			else if (angl < 0 && angr > 0)
				direction = 1;
			else
				direction = 0;
		}
		else
			direction = 0;

		//if (playback)
		//	cout << direction << " - " << left_lane << " & " << right_lane << endl;

		prev_offset = 80 * direction;

		int nonzero = 0;
		if (direction == 0)
		{
			if (left_lane || right_lane)
			{
				for (nonzero = 0; nonzero < colorMask.cols; nonzero++)
					if (colorMask.at<unsigned char>(colorMask.rows - 2, nonzero) > 0)
						break;
				offset = lane_width / 2 + nonzero - colorMask.cols / 2;
			}
			else
			{
				offset = 0;
			}
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

		//line(original, Point(original.cols / 2 + offset, original.rows - 5), Point(original.cols / 2, original.rows - 5), 150, 5);
		line(colorMask, Point(colorMask.cols / 2 + offset, colorMask.rows - 5), Point(colorMask.cols / 2, colorMask.rows - 5), 150, 5);
		imshow("img", colorMask);

		if (waitKey(5) == 'p')
			playback = !playback;
	}
	std::cout << "finished" << endl;
	return 0;
}