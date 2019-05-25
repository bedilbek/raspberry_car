#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include "raspicam/raspicam_cv.h"
#include <fstream>
#include <ctime>
#include <csignal>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace raspicam;
using namespace cv;

int video_frame_rate = 24;
int video_frame_width = 640;
int video_frame_height = 480;

RaspiCam_Cv raspiCam_cv;
VideoWriter videoWriter;

void  signal_handler(int signum)
{

    cout << "Interrupt signal - " << signum << " received.\n";

    raspiCam_cv.release();
    videoWriter.release();
    cout << "Everything is saved" << endl;
    exit(signum);
}


int main() {
    signal(SIGINT, signal_handler);

    cv::Mat image;


    raspiCam_cv.set(CAP_PROP_FPS, video_frame_rate);
    raspiCam_cv.set(CAP_PROP_FRAME_HEIGHT, video_frame_height);
    raspiCam_cv.set(CAP_PROP_FRAME_WIDTH, video_frame_width);
    raspiCam_cv.setVideoStabilization(true);
    videoWriter = VideoWriter();
    raspiCam_cv.open();

    double frame_rate = raspiCam_cv.get(CAP_PROP_FPS);
    double frame_width = raspiCam_cv.get(CAP_PROP_FRAME_WIDTH);
    double frame_height = raspiCam_cv.get(CAP_PROP_FRAME_HEIGHT);

    videoWriter.open("output.avi", CV_FOURCC('M', 'J', 'P', 'G'), frame_rate, cv::Size(frame_width, frame_height), true);

    while (1) {
        raspiCam_cv.grab();
        raspiCam_cv.retrieve(image);
        videoWriter.write(image);
    }

    return 0;
}


