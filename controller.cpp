//
// Created by Sardor on 2019-03-11.
//

#include "controller.h"
#include <cstdio>
#include <wiringPi.h>
#include <softPwm.h>

#define IN1_PIN 1
#define IN2_PIN 4
#define IN3_PIN 5
#define IN4_PIN 6

void Controller::init_dc_motor() {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, HIGH);
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, HIGH);

    softPwmCreate(IN1_PIN, MIN_SPEED, MAX_SPEED);
    softPwmCreate(IN2_PIN, MIN_SPEED, MAX_SPEED);
    softPwmCreate(IN3_PIN, MIN_SPEED, MAX_SPEED);
    softPwmCreate(IN4_PIN, MIN_SPEED, MAX_SPEED);

}

void Controller::set_pin_mode() {
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(IN3_PIN, OUTPUT);
    pinMode(IN4_PIN, OUTPUT);
}


void Controller::forward(int speed) {
    if (speed > MAX_SPEED)
        speed = MAX_SPEED;
    if (speed < 10)
        speed = 20;
    softPwmWrite(IN1_PIN, speed);
    softPwmWrite(IN2_PIN, MIN_SPEED);
    softPwmWrite(IN3_PIN, speed);
    softPwmWrite(IN4_PIN, MIN_SPEED);

}

void Controller::backward(int speed) {
    if (speed > MAX_SPEED)
        speed = MAX_SPEED;
    if (speed < 10)
        speed = 20;
    softPwmWrite(IN1_PIN, LOW);
    softPwmWrite(IN2_PIN, speed);
    softPwmWrite(IN3_PIN, LOW);
    softPwmWrite(IN4_PIN, speed);
}

void Controller::left(int speed) {
    if (speed > MAX_SPEED)
        speed = MAX_SPEED;
    if (speed < 10)
        speed = 20;
    softPwmWrite(IN1_PIN, LOW);
    softPwmWrite(IN2_PIN, speed);
    softPwmWrite(IN3_PIN, speed);
    softPwmWrite(IN4_PIN, LOW);
}

void Controller::right(int speed) {
    if (speed > MAX_SPEED)
        speed = MAX_SPEED;
    if (speed < 10)
        speed = 20;
    softPwmWrite(IN1_PIN, speed);
    softPwmWrite(IN2_PIN, LOW);
    softPwmWrite(IN3_PIN, LOW);
    softPwmWrite(IN4_PIN, speed);
}

void Controller::stop() {
    digitalWrite(IN1_PIN,LOW);
    digitalWrite(IN2_PIN,LOW);
    digitalWrite(IN3_PIN,LOW);
    digitalWrite(IN4_PIN,LOW);

}