#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "lane_detector/binarization_utils.h"
#include <iostream>

using namespace cv;
using namespace std;
int h = 0;
int s = 0;
int v=0;
int hm = 0;
int sm = 0;
int vm =0;

Scalar min_th = { 0, 0, 235 };
Scalar max_th = { 100, 0, 255 };

void update(int, void*)
{
    min_th = Scalar(h, s, v);
    max_th = Scalar(hm, sm, vm);
}

Mat image;

int main() {
    image = imread("../files/zebra-test.jpg", IMREAD_COLOR);
    if (image.empty()) {
        return 0;
    }

    namedWindow("Controls", WINDOW_GUI_NORMAL);
    createTrackbar("H", "Controls", &h, 255, update);
    createTrackbar("S", "Controls", &s, 255, update);
    createTrackbar("V", "Controls", &v, 255, update);

    createTrackbar("HM", "Controls", &hm, 255, update);
    createTrackbar("SM", "Controls", &sm, 255, update);
    createTrackbar("VM", "Controls", &vm, 255, update);

    Mat gray;
    imshow("org", image);
    image.convertTo(image, -1, 1.3, 10);

    Mat hsv = thresh_frame_in_HSV(image, min_th, max_th, false);

    Canny(hsv, gray, 100, 200, 3);
    imshow("Canny", gray);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    findContours(gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    Mat drawing = Mat::zeros(gray.size(), CV_8UC3);
    int count = 0;
    for (int i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        double matcher = contourArea(contours[i], false);
        Moments M = moments(contours[i]);
        int cY = (int) (M.m01 / M.m00);
        if (matcher > 1500) {
            for (int j = i + 1; j < contours.size(); j++) {
                Moments M2 = moments(contours[j]);
                int cY2 = (int) (M2.m01 / M2.m00);
                if (abs(cY-cY2) < 10) {
                    count++;
                }
            }
        }
    }
    cout << count << endl;
    waitKey(0);
    return 0;
}

