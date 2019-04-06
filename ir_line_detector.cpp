//
// Created by bedilbek on 4/6/19.
//

#include "ir_line_detector.h"
#include <wiringPi.h>

IRLineDetector::IRLineDetector(){
    init();
}

void IRLineDetector::init() {
    pinMode(IR_Line_LEFT, INPUT);
    pinMode(IR_Line_RIGHT, INPUT);
}

bool IRLineDetector::left_detected() {
    return !static_cast<bool>(digitalRead(IR_LINE_LEFT));
}

bool IRLineDetector::right_detected() {
    return !static_cast<bool>(digitalRead(IR_LINE_RIGHTT));
}

