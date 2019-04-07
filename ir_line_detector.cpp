//
// Created by bedilbek on 4/6/19.
//

#include "ir_line_detector.h"
#include <wiringPi.h>
#include <iostream>

IRLineDetector::IRLineDetector(){
    init();
}

void IRLineDetector::init() {
    pinMode(IR_LINE_LEFT, INPUT);
    pinMode(IR_LINE_RIGHT, INPUT);
}

bool IRLineDetector::left_detected() {
//    std::cout << "LEFT LINE:" << digitalRead(IR_LINE_LEFT) << std::endl;
    return static_cast<bool>(digitalRead(IR_LINE_LEFT));
}

bool IRLineDetector::right_detected() {
//    std::cout << "RIGHT LINE:" << digitalRead(IR_LINE_RIGHT) << std::endl;
    return static_cast<bool>(digitalRead(IR_LINE_RIGHT));
}

