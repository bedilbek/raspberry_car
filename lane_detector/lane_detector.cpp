#include "binarization_utils.h"
#include "bird_eye.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main()
{
	Mat img = imread("test.jpg");
	Mat result = binarize(img, true);
	Mat forw, back;

	Mat bird = bird_eye(img, forw, back, false);

	namedWindow("image", WINDOW_NORMAL);
	imshow("bird", bird);
	imshow("norm", img);
	waitKey(0);
	return 0;
}