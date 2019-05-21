#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include "raspicam/raspicam_cv.h"
#include <fstream>
#include <ctime>
#include "opencv2/opencv.hpp"

using namespace std;

int main() {
    cv::Mat image;

    raspicam::RaspiCam_Cv camera;
    camera.set(CV_CAP_PROP_FORMAT, CV_8UC1);
    camera.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    camera.set(cv::CAP_PROP_FPS, 15);

    camera.open();

    int frame_width = camera.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height = camera.get(CV_CAP_PROP_FRAME_HEIGHT);
    int frame_rate = camera.get(CV_CAP_PROP_FPS);

    cv::VideoWriter video("output.avi", CV_FOURCC('M', 'J', 'P', 'G'), frame_rate, cv::Size(frame_width, frame_height), true);

    if (!camera.isOpened() || !video.isOpened()) {
        std::cout << "Could not initialize capturing or writing...\n";
        std::cout << std::endl;
        return 0;
    }

    video.release();

    while (1) {
        camera.grab();
        camera.retrieve(image);
//        cout << "rows: " << image.rows << endl << "cols: " << image.cols << endl;
        cv::imshow("hello", image);
		
        char c = (char) cv::waitKey(1);
        if (c == 27) break;
    }
    camera.release();
    video.release();

    return 0;
}


