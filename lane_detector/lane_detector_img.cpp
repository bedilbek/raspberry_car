// LaneDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "binarization_utils.h"
#include "bird_eye.h"
#include "line_utils.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat frame, result;

int hue_low = 0;
int hue_high = 0;
int saturation_low = 0;
int saturation_high = 0;
int brightness_high = 0;
int brightness_low = 0;
int const max_hsv_value = 255;
char* window_name = "HSV Fixation";

void on_hsv_change(int, void *)
{
    cout << "HUE LOW" << hue_low << endl;
    cout << "HUE HIGH" << hue_high << endl;
    cout << "SATURATION HIGH" << saturation_high << endl;
    cout << "SATURATION LOW" << saturation_low << endl;
    cout << "BRIGHTNESS HIGH" << brightness_high << endl;
    cout << "BRIGHTNESS LOW" << brightness_low << endl;

    result = thresh_frame_in_HSV(frame, Scalar(hue_low, saturation_low, brightness_low), Scalar(hue_high, saturation_high, brightness_high), false);

    imshow(window_name, result);
    char c = (char) waitKey(25);
    if (c == 27) exit(0);
}


int main()
{
	frame = imread("files/blur_turn.png");

	namedWindow(window_name, WINDOW_AUTOSIZE);
//	Mat binarized = binarize(frame, true);
//	Mat forward, backward;
//	Mat birdeye = bird_eye(binarized, forward, backward, false);
//	Mat birdeye_org = bird_eye(frame, forward, backward, false);
//
//	auto lines = get_fits_by_sliding_windows(birdeye);
//	double offset = compute_offset_from_center(lines[0], lines[1], frame.cols);
//
//	resize(birdeye, birdeye, Size(birdeye.cols / 2, birdeye.rows / 2));
//	resize(birdeye_org, birdeye_org, Size(birdeye.cols / 2, birdeye.rows / 2));
//	imshow("Bin", binarized);


    createTrackbar("HUE LOW", window_name, &hue_low, max_hsv_value, on_hsv_change);
    createTrackbar("SATURATION LOW", window_name, &saturation_low, max_hsv_value, on_hsv_change);
    createTrackbar("BRIGHTNESS LOW", window_name, &brightness_low, max_hsv_value, on_hsv_change);
    createTrackbar("HUE HIGH", window_name, &hue_high, max_hsv_value, on_hsv_change);
    createTrackbar("SATURATION HIGH", window_name, &saturation_high, max_hsv_value, on_hsv_change);
    createTrackbar("BRIGHTNESS HIGH", window_name, &brightness_high, max_hsv_value, on_hsv_change);

    on_hsv_change(0, 0);
    imshow("Org", frame);
    waitKey(0);
    return 0;
}

// Modify the program such that: 
// 1. It takes raspicam input frames instead of imread
// 2. When red traffic signs are identified, then the motors should act accordingly (for that use the motor control functions such as goForward(), goRight(), stop(), goLeft() etc.)