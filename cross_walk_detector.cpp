//
// Created by Sardor on 2019-04-21.
//

#include "cross_walk_detector.h"

bool CrossWalkDetector::hasCrossWalk(Mat image) {
    if (image.empty()) {
        return false;
    }
    Mat gray;
    image.convertTo(image, -1, 1.3, 10);
    Mat hsv = thresh_frame_in_HSV(image, Scalar(0, 0, 235), Scalar(100, 0, 255), false);
    Canny(hsv, gray, 100, 200, 3);
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
    return count >= 3;
}


