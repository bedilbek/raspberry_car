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
    if (steering > 0) {
        float x = 100.0 - steering;
        softPwmWrite(IN1_PIN, x);
        softPwmWrite(IN2_PIN, MIN_SPEED);
        softPwmWrite(IN3_PIN, speed);
        softPwmWrite(IN4_PIN, MIN_SPEED);
    } else {
        steering = -steering;
        float y = 100.0 - steering;
        softPwmWrite(IN1_PIN, speed);
        softPwmWrite(IN2_PIN, MIN_SPEED);
        softPwmWrite(IN3_PIN, y);
        softPwmWrite(IN4_PIN, MIN_SPEED);
    }

}

void Controller::forward(int speed) {
    softPwmWrite(IN1_PIN, speed);
    softPwmWrite(IN2_PIN, MIN_SPEED);
    softPwmWrite(IN3_PIN, speed);
    softPwmWrite(IN4_PIN, MIN_SPEED);
}

void Controller::backward(int speed) {
    softPwmWrite(IN1_PIN, MIN_SPEED);
    softPwmWrite(IN2_PIN, speed);
    softPwmWrite(IN3_PIN, MIN_SPEED);
    softPwmWrite(IN4_PIN, speed);
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