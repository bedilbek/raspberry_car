#include <cstdio>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <softPwm.h>
#include <zconf.h>
#include "libSonar.h"
#include "controller.h"
#include "ir_wall_detector.h"
#include "ir_line_detector.h"
#include <thread>
#include <stdlib.h>

#include <csignal>
#include "datagram_socket_server.h"

using namespace std;
using namespace raspicam;
using namespace cv;

Controller controller;
RaspiCam_Cv raspiCam_cv;
VideoWriter videoWriter;
int frame_counter = 0;

void  signal_handler(int signum)
{
    cout << "Interrupt signal - " << signum << " received.\n";

    controller.stop();
    raspiCam_cv.release();
    videoWriter.release();
    // cleanup and close up stuff here
    // terminate program
    cout << "Everything is saved" << endl;
    exit(signum);
}

void video_writer()
{
    raspiCam_cv.open();

    double frame_rate = raspiCam_cv.get(CAP_PROP_FPS);
    double frame_width = raspiCam_cv.get(CAP_PROP_FRAME_WIDTH);
    double frame_height = raspiCam_cv.get(CAP_PROP_FRAME_HEIGHT);

    videoWriter = VideoWriter("output.avi", CV_FOURCC('M', 'J', 'P', 'G'), frame_rate, cv::Size(frame_width, frame_height), true);

    Mat img;

    while (true) {

        raspiCam_cv.grab();
        raspiCam_cv.retrieve(img);
        frame_counter++;
        videoWriter.write(img);
    }

}


int main() {
    datagram_socket_server *s = new datagram_socket_server(8059, "localhost", true, true);
    char message[1024];

    //  setup signal handler for SIGINT
    signal(SIGINT, signal_handler);

    std::thread v(video_writer);
//    namedWindow("hello", WINDOW_NORMAL);

    if (wiringPiSetup() == -1)
    {
        cout << "wiringPi is not set up properly" << endl;
        return -1;
    }

    int steering = 0;
    int speed = 0;

    controller.init_dc_motor();
    cout << "ready to receive" << endl;
    while (1)
    {
        s->receive(message, 1024);
        speed = (int)message[0] == 0 ? ((int)message[1]) * -1 : (int) message[1];
        steering = (int)message[2] == 0 ? ((int)message[3]) : (int) message[3] * -1;
        controller.turn(steering, speed);
//        cout <<message << "::::" << speed << "." << steering <<  endl;
        delay(20);
    }

//    char input = '0';
//    while (true) {
//        input = static_cast<char>(waitKey(15));
//        switch (input) {
//            case 'w': {
//                speed += 15;
//                if (speed >= 100)
//                    speed = 100;
//                break;
//            }
//            case 'd': {
//                steering -= 15;
//                if (steering <= -100)
//                    steering = -100;
//                break;
//            }
//            case 'a': {
//                steering += 15;
//                if (steering >= 100)
//                    steering = 100;
//                break;
//
//            }
//            case 's': {
//                speed -= 15;
//                if (speed <= -100)
//                    speed = -100;
//                break;
//            }
//            default: {
//                break;
//            }
//        }
//        cout << input << "speed: " << speed << " steering: " << steering << endl;

//        cout<<"frame: "<<frame_counter<<" steering: [" << 100.0 - steering << "] speed: [" << speed <<"]" << endl;



    return 0;
}

