#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include <fstream>
#include <ctime>
#include "ir_wall_detector.h"

int main() {
    if (wiringPiSetup() == -1)
        return -1;

    IRWallDetector ir_wall_detect;

    while (1) {

        if (ir_wall_detect.left_detected()) {
            std::cout << "Move Right" << std::endl;
        }

        if (ir_wall_detect.right_detected()) {
            std::cout << "Move Left" << std::endl;
        }

    }
}
