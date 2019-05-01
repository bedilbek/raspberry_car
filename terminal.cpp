#include <cstdio>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
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



using namespace std;
using namespace raspicam;
using namespace cv;

int main() {

    namedWindow("hello", WINDOW_NORMAL);
    int speed = 0;
    int steering = 0;

    char input = '0';
    while (true) {
        input = static_cast<char>(waitKey(15));
        switch (input) {
            case 'w': {
                speed += 15;
                if (speed >= 100)
                    speed = 100;
                break;
            }
            case 'a': {
                steering -= 15;
                if (steering <= -100)
                    steering = -100;
                break;
            }
            case 'd': {
                steering += 15;
                if (steering >= 100)
                    steering = 100;
                break;

            }
            case 's': {
                speed -= 15;
                if (speed <= 0)
                    speed = 0;
                break;
            }
            default: {
                break;
            }
        }
        cout << input << "speed: " << speed << " steering: " << steering << endl;
    }

    return 0;
}

