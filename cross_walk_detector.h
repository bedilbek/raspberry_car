//
// Created by Sardor on 2019-04-21.
//

#ifndef SMART_CAR_CROSS_WALK_DETECTOR_H
#define SMART_CAR_CROSS_WALK_DETECTOR_H

#include "cross_walk_detector.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "lane_detector/binarization_utils.h"
#include <iostream>

using namespace cv;
using namespace std;

class CrossWalkDetector {
public:
    bool hasCrossWalk(Mat image);
};

#endif //SMART_CAR_CROSS_WALK_DETECTOR_H
