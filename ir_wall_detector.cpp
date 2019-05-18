//
// Created by bedilbek on 4/6/19.
//

#include "ir_wall_detector.h"
#include <wiringPi.h>
#include <iostream>

// when there is obstacle receives 0
// when there is no obstacle receives 1

IRWallDetector::IRWallDetector(){

}

void IRWallDetector::init() {
    pinMode(IR_WALL_LEFT, INPUT);
    pinMode(IR_WALL_RIGHT, INPUT);
}

bool IRWallDetector::left_detected() {
//    std::cout << "LEFT OBSTACLE:" << digitalRead(IR_WALL_LEFT) << std::endl;
    return !static_cast<bool>(digitalRead(IR_WALL_LEFT));
}

bool IRWallDetector::right_detected() {
//    std::cout << "RIGHT OBSTACLE:" << digitalRead(IR_WALL_RIGHT) << std::endl;
    return !static_cast<bool>(digitalRead(IR_WALL_RIGHT));
}

