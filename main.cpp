#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include "raspicam/raspicam_cv.h"
#include <fstream>
#include <ctime>
#include "opencv2/opencv.hpp"

#define IN1_PIN 1
#define IN2_PIN 4
#define IN3_PIN 5
#define IN4_PIN 6

void init_dc_motor();
void go_backward();
void go_forward();
void go_left();
void go_right();
void stop_dc_motor();

int main() {

    cv::Mat image, output;

    raspicam::RaspiCam_Cv camera;
    camera.set( CV_CAP_PROP_FORMAT, CV_8UC1 );
    camera.set ( cv::CAP_PROP_FRAME_WIDTH,  160);
    camera.set ( cv::CAP_PROP_FRAME_HEIGHT, 120);
    camera.set ( cv::CAP_PROP_BRIGHTNESS, 35);
    camera.set ( cv::CAP_PROP_CONTRAST , 35);
    camera.set ( cv::CAP_PROP_SATURATION, 35);
    camera.set ( cv::CAP_PROP_GAIN,  35);
    camera.set ( cv::CAP_PROP_FPS, 12);
    camera.open();

    if( !camera.isOpened() )
    {
        std::cout << "Could not initialize capturing...\n";
        return 0;
    }

    while(1){
        camera.grab();
        camera.retrieve(output);

        imshow("webcam input", output);
        char c = (char)cv::waitKey(10);
        if( c == 27 ) break;
    }}


void init_dc_motor()
{
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, HIGH);
}

void go_backward()
{
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, HIGH);
}

void go_forward()
{
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, LOW);
}

void go_left()
{
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, LOW);
}

void go_right()
{
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, HIGH);
}

void stop_dc_motor()
{
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, LOW);
}