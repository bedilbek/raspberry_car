#include "binarization_utils.h"

using namespace cv;

Scalar yellow_HSV_th_min = { 18, 148, 148 };
Scalar yellow_HSV_th_max = { 37, 255, 255 };

//Threshold a color frame in HSV space
Mat thresh_frame_in_HSV(Mat3b frame, Scalar min_values, Scalar max_values, bool verbous = false)
{
	Mat hsv;
	cvtColor(frame, hsv, COLOR_BGR2HSV);

	Mat maskHSV;
	inRange(hsv, min_values, max_values, maskHSV);

	return maskHSV;
}

Mat thresh_frame_in_LAB(Mat3b frame, Scalar min_values, Scalar max_values, bool verbous = false)
{
	Mat hsv;
	cvtColor(frame, hsv, COLOR_BGR2Lab);

	Mat maskHSV, result_hsv;
	inRange(hsv, min_values, max_values, maskHSV);

	return maskHSV;
}

//Apply Sobel edge detection to an input frame, then threshold the result
Mat thresh_frame_sobel(Mat frame, int kernel_size)
{
	Mat gray, blur;
	GaussianBlur(frame, blur, cv::Size(3, 3), 0);

	cvtColor(blur, gray, COLOR_BGR2GRAY);

	Mat sobel_x, sobel_y, sobel, grad;
	Sobel(gray, sobel_x, CV_64F, 1, 0, kernel_size);
	Sobel(gray, sobel_y, CV_64F, 0, 1, kernel_size);


	Mat sobel_x_p, sobel_y_p, sobel_sum, sobel_sqrt;
	pow(sobel_x, 2, sobel_x_p);
	pow(sobel_y, 2, sobel_y_p);
	add(sobel_x_p, sobel_y_p, sobel_sum);
	sqrt(sobel_sum, sobel_sqrt);
	convertScaleAbs(sobel_sqrt, sobel);

//	threshold(sobel, grad, 150, 256, THRESH_BINARY);
	adaptiveThreshold(sobel, grad, 200, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 0);
	
	return grad;
}

//Apply Canny edge detection to an input frame, then threshold the result
Mat thresh_frame_canny(Mat frame, int kernel_size)
{
	Mat gray, blur;
	GaussianBlur(frame, blur, cv::Size(3, 3), 0);

	cvtColor(blur, gray, COLOR_BGR2GRAY);

	Mat sobel_x, sobel_y, sobel, grad;
	Sobel(gray, sobel_x, CV_64F, 1, 0, kernel_size);
	Sobel(gray, sobel_y, CV_64F, 0, 1, kernel_size);


	Mat sobel_x_p, sobel_y_p, sobel_sum, sobel_sqrt;
	pow(sobel_x, 2, sobel_x_p);
	pow(sobel_y, 2, sobel_y_p);
	add(sobel_x_p, sobel_y_p, sobel_sum);
	sqrt(sobel_sum, sobel_sqrt);
	convertScaleAbs(sobel_sqrt, sobel);

	threshold(sobel, grad, 50, 256, THRESH_BINARY);

	return grad;
}

//Apply histogram equalization to an input frame, threshold it and return the (binary) result.
Mat get_binary_from_equalized_grayscale(Mat frame)
{
	Mat gray, eq_glob;
	cvtColor(frame, gray, COLOR_BGR2GRAY);
	equalizeHist(gray, eq_glob);


	Mat th;
	threshold(eq_glob, th, 250, 255, THRESH_BINARY);
	return th;
}


//Convert an input frame to a binary image which highlight as most as possible the lane-lines.
//:param img : input color frame
Mat binarize(Mat img, bool verbous = false)
{
	Mat binary, yellow_mask, eq_white_mask, sobel_mask;

	binary = Mat(img.rows, img.cols, CV_8UC1, Scalar(0));
	
	yellow_mask = thresh_frame_in_HSV(img, yellow_HSV_th_min, yellow_HSV_th_max, verbous);
	bitwise_or(yellow_mask, binary, binary);

	eq_white_mask = get_binary_from_equalized_grayscale(img);
	bitwise_or(eq_white_mask, binary, binary);

	sobel_mask = thresh_frame_sobel(img, 3);
	bitwise_or(sobel_mask, binary, binary);

	Mat result;
	Mat kernel = Mat(5, 5, CV_8UC1, Scalar(1));
	morphologyEx(binary, result, MORPH_CLOSE, kernel);

	//namedWindow("sobel mask", WINDOW_NORMAL);
	//imshow("sobel mask", sobel_mask);

	if (verbous)
	{
		//namedWindow("yellow mask", WINDOW_NORMAL);
		//imshow("yellow mask", yellow_mask);

		//namedWindow("eq white mask", WINDOW_NORMAL);
		//imshow("eq white mask", eq_white_mask);

		namedWindow("sobel mask", WINDOW_NORMAL);
		imshow("sobel mask", sobel_mask);
	}

	return result;
}