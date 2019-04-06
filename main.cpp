#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include <fstream>
#include <ctime>
#include "ir_wall_detector.h"
#include "ir_line_detector.h"

int main() {
    if (wiringPiSetup() == -1)
        return -1;

    IRWallDetector ir_wall_detect;
    IRLineDetector ir_line_detect;

    while (1) {

        if (ir_wall_detect.left_detected()) {
            std::cout << "Left Obstacle" << std::endl;
        }

        if (ir_wall_detect.right_detected()) {
            std::cout << "Right Obstacle" << std::endl;
        }

        if (ir_line_detect.right_detected()) {
            std::cout << "Right Line" << std::endl;
        }

        if (ir_line_detect.left_detected()) {
            std::cout << "Left Line" << std::endl;
        }

    }
}
