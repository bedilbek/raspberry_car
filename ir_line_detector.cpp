//
// Created by bedilbek on 4/6/19.
//

#include "ir_line_detector.h"
#include <wiringPi.h>
#include <iostream>

IRLineDetector::IRLineDetector(){
}

void IRLineDetector::init() {
    pinMode(IR_LINE_LEFT, INPUT);
    pinMode(IR_LINE_RIGHT, INPUT);
}

bool IRLineDetector::left_detected() {
//    std::cout << "L=" << digitalRead(IR_LINE_LEFT);
    return static_cast<bool>(digitalRead(IR_LINE_LEFT));
}

bool IRLineDetector::right_detected() {
//    std::cout << " R=" << digitalRead(IR_LINE_RIGHT) << std::endl;
    return static_cast<bool>(digitalRead(IR_LINE_RIGHT));
}

bool IRLineDetector::isLeftOnBlack(){
    //if left line is black
    if(digitalRead(IR_LINE_LEFT) == HIGH){
        return true;
    }
    return false;
}
bool IRLineDetector::isRightOnBlack(){
    //if right line is black
    if(digitalRead(IR_LINE_RIGHT) == HIGH){
        return true;
    }
    return false;
}


