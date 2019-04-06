#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include "raspicam/raspicam_cv.h"
#include <fstream>
#include <ctime>
#include "opencv2/opencv.hpp"

using namespace std;

int main() {
    cv::Mat image, output;

    raspicam::RaspiCam_Cv camera;
    camera.set(CV_CAP_PROP_FORMAT, CV_8UC1);
    camera.set(cv::CAP_PROP_FRAME_WIDTH, 160);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 120);
    camera.set(cv::CAP_PROP_BRIGHTNESS, 35);
    camera.set(cv::CAP_PROP_CONTRAST, 35);
    camera.set(cv::CAP_PROP_SATURATION, 35);
    camera.set(cv::CAP_PROP_GAIN, 35);
    camera.set(cv::CAP_PROP_FPS, 12);

    camera.open();

    if (!camera.isOpened()) {
        std::cout << "Could not initialize capturing...\n";
        return 0;
    }

    while (1) {
        camera.grab();
        camera.retrieve(output);

        imshow("webcam input", output);
        char c = (char) cv::waitKey(10);
        if (c == 27) break;

    }
}

