#include "binarization_utils.h"
#include "bird_eye.h"
#include "line_utils.h"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main()
{
    Mat img = imread("files/testing_road1.png", IMREAD_COLOR);

    Mat img_resized;
    resize(img, img_resized, Size(1280, 720));

    Mat img_bin = binarize(img_resized, false);
    Mat forw, back;

    Mat img_bird = bird_eye(img_bin, forw, back, false);

    array<Line, 2> lines = get_fits_by_sliding_windows(img_bird);

    cout << lines[0].curvature() << endl;
    get_fits_by_previous_fits(img_bird, lines[0], lines[1], false);

	return 0;
}