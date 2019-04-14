//
// Created by bedilbek on 4/14/19.
//

#ifndef SMART_CAR_BIRD_EYE_H
#define SMART_CAR_BIRD_EYE_H

#include <opencv2/opencv.hpp>
using namespace cv;

Mat bird_eye(Mat img, Mat forw_mat, Mat backw_mat, bool verbose);

#endif //SMART_CAR_BIRD_EYE_H
