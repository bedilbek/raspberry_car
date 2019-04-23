//
// Created by Sardor on 2019-03-11.
//

#include "controller.h"

#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define IN1_PIN 1
#define IN2_PIN 4
#define IN3_PIN 5
#define IN4_PIN 6
#define MAX_SPEED 100
#define MIN_SPEED 0

void Controller::init_dc_motor() {

    pinMode(IN1_PIN, SOFT_PWM_OUTPUT);
    pinMode(IN2_PIN, SOFT_PWM_OUTPUT);
    pinMode(IN3_PIN, SOFT_PWM_OUTPUT);
    pinMode(IN4_PIN, SOFT_PWM_OUTPUT);

    softPwmCreate(IN1_PIN, 0, 100);
    softPwmCreate(IN2_PIN, 0, 100);
    softPwmCreate(IN3_PIN, 0, 100);
    softPwmCreate(IN4_PIN, 0, 100);
}

void Controller::turn(int steering,int speed) {
    int left,right;

    if(speed == 0)
        stop();

    else if(speed>0){
        //forward
        if(steering>0){
            left = speed -steering;
            right = speed;
        }else{
            steering = -steering;
            left = speed;
            right = speed - steering;
        }
        forward(left,right);
    } else{
        //backward
        speed =-speed;
        if(steering>0){
            left = speed -steering;
            right = speed;
        }else{
            steering = -steering;
            left = speed;
            right = speed - steering;
        }
        backward(left,right);
    }
}


void Controller::forward(int left,int right) {
    softPwmWrite(IN1_PIN, left);
    softPwmWrite(IN2_PIN, MIN_SPEED);
    softPwmWrite(IN3_PIN, right);
    softPwmWrite(IN4_PIN, MIN_SPEED);
}

void Controller::backward(int left,int right) {
    softPwmWrite(IN1_PIN, MIN_SPEED);
    softPwmWrite(IN2_PIN, left);
    softPwmWrite(IN3_PIN, MIN_SPEED);
    softPwmWrite(IN4_PIN, right);
}

void Controller::left(int speed) {
    softPwmWrite(IN1_PIN, MIN_SPEED);
    softPwmWrite(IN2_PIN, speed);
    softPwmWrite(IN3_PIN, speed);
    softPwmWrite(IN4_PIN, MIN_SPEED);
}

void Controller::right(int speed) {
    softPwmWrite(IN1_PIN, speed);
    softPwmWrite(IN2_PIN, MIN_SPEED);
    softPwmWrite(IN3_PIN, MIN_SPEED);
    softPwmWrite(IN4_PIN, speed);
}

void Controller::stop() {
    softPwmWrite(IN1_PIN, LOW);
    softPwmWrite(IN2_PIN, LOW);
    softPwmWrite(IN3_PIN, LOW);
    softPwmWrite(IN4_PIN, LOW);

    init_dc_motor();
}