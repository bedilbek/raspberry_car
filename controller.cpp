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

void Controller::go_backward_for_2_right_smooth_stop() {
    backward(80);
    delay(2000);
    right_smooth(100);
    stop();
}

void Controller::right_smooth(int speed) {
    softPwmWrite(IN1_PIN, speed + 10);
    softPwmWrite(IN2_PIN, MIN_SPEED);
    softPwmWrite(IN3_PIN, MIN_SPEED);
    softPwmWrite(IN4_PIN, speed - 10);
}


void Controller::go_forward_for_2_left_point_stop() {
    forward(80);
    delay(2000);
    left(100);
    delay(900);
    stop();
}


void Controller::turn_right_go_back_for_3_stop() {
    right(100);
    delay(500);
    stop();
    backward(80);
    delay(3000);
    stop();
}

void Controller::turn_left_go_forward_for_3_stop() {
    left(100);
    delay(500);
    stop();
    forward(80);
    delay(3000);
    stop();
}

void Controller::go_forward_for_5() {
    forward(80);
    delay(5000);
}

void Controller::forward(int left, int right)
{
	softPwmWrite(IN1_PIN, left);
	softPwmWrite(IN2_PIN, MIN_SPEED);
	softPwmWrite(IN3_PIN, right);
	softPwmWrite(IN4_PIN, MIN_SPEED);
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
    softPwmWrite(IN1_PIN, 0);
    softPwmWrite(IN2_PIN, 0);
    softPwmWrite(IN3_PIN, 0);
    softPwmWrite(IN4_PIN, 0);

    init_dc_motor();
}