//// LaneDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
//#include "pch.h"
//#include "binarization_utils.h"
//#include "bird_eye.h"
//#include "line_utils.h"
//#include <opencv2/opencv.hpp>
//#include <iostream>
//#include <vector>
//
//using namespace std;
//using namespace cv;
//
//int main()
//{
//	VideoCapture cap("test.mp4");
//
//	if (!cap.isOpened()) {
//		cout << "Error opening video stream or file" << endl;
//		return -1;
//	}
//	bool playback = true;
//	while (1) 
//	{
//		if (playback)
//		{
//			Mat frame;
//			// Capture frame-by-frame
//			cap >> frame;
//
//			// If the frame is empty, break immediately
//			if (frame.empty())
//				break;
//
//			Mat binarized = binarize(frame, true);
//			Mat forward, backward;
//			Mat birdeye = bird_eye(binarized, forward, backward, false);
//			Mat birdeye_org = bird_eye(frame, forward, backward, false);
//
//			//auto lines = get_fits_by_sliding_windows(birdeye);
//			//double offset = compute_offset_from_center(lines[0], lines[1], frame.cols);
//
//			resize(birdeye, birdeye, Size(birdeye.cols / 2, birdeye.rows / 2));
//			//resize(birdeye_org, birdeye_org, Size(birdeye.cols / 2, birdeye.rows / 2));
//			imshow("Bin", birdeye);
//			imshow("Org", birdeye_org);
//
//
//			// Display the resulting frame
//			//imshow("Frame", frame);
//		}
//		// Press  ESC on keyboard to exit
//		char c = (char)waitKey(25);
//		if (c == 27)
//			break;
//		if (c == 'p')
//			playback = !playback;
//	}
//
//	// When everything done, release the video capture object
//	cap.release();
//
//	// Closes all the frames
//	destroyAllWindows();
//
//	return 0;
//}

// Modify the program such that: 
// 1. It takes raspicam input frames instead of imread
// 2. When red traffic signs are identified, then the motors should act accordingly (for that use the motor control functions such as goForward(), goRight(), stop(), goLeft() etc.)

#include "opencv2/opencv.hpp" 
#include <iostream>  
#include <string> 

using namespace cv;
using namespace std;
int main()
{
	Mat bgr_image = imread("../files/testing_road1.png", IMREAD_COLOR);
	Mat hsv_image;
	cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);
	// Threshold the HSV image, keep only the red pixels
	Mat lower_red_hue_range;
	Mat upper_red_hue_range;
	inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
	inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);
	// Threshold the HSV image, keep only the blue pixels
	Mat blue_hue_range;
	inRange(hsv_image, cv::Scalar(80, 65, 65, 0), cv::Scalar(140, 255, 255, 0), blue_hue_range);
	imshow("Original", bgr_image);
	waitKey(0);
	// Use the Hough transform to detect circles in the threshold image
	vector<cv::Vec3f> circles;
	HoughCircles(lower_red_hue_range, circles, CV_HOUGH_GRADIENT, 1, lower_red_hue_range.rows / 8, 100, 20, 0, 0);
	HoughCircles(blue_hue_range, circles, CV_HOUGH_GRADIENT, 1, blue_hue_range.rows / 8, 100, 20, 0, 0);
	// Loop over all detected circles and outline them on the original image
	if (circles.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);

		circle(bgr_image, center, radius, cv::Scalar(0, 255, 0), 5);
	}
	Mat inv_lower_red_hue_range = Scalar::all(255) - lower_red_hue_range;
	Mat inv_blue_hue_range = Scalar::all(255) - blue_hue_range;
	imshow("Reds", inv_lower_red_hue_range);
	imshow("Blues", inv_blue_hue_range);

	waitKey(0);
	return 0;
}