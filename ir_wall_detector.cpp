//
// Created by bedilbek on 4/6/19.
//

#include "ir_wall_detector.h"
#include <wiringPi.h>

IRWallDetector::IRWallDetector(){
    init();
}

void IRWallDetector::init() {
    pinMode(IR_WALL_LEFT, INPUT);
    pinMode(IR_WALL_RIGHT, INPUT);
}

bool IRWallDetector::left_detected() {
    return !static_cast<bool>(digitalRead(IR_WALL_LEFT));
}

bool IRWallDetector::right_detected() {
    return !static_cast<bool>(digitalRead(IR_WALL_RIGHT));
}

