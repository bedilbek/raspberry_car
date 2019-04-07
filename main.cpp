#include <cstdio>
#include <iostream>
#include <wiringPi.h>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <zconf.h>
#include "libSonar.h"
#include "controller.h"
#include "ir_wall_detector.h"
#include "ir_line_detector.h"

using namespace std;

int main() {
    if (wiringPiSetup() == -1)
        return 0;

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
    return 0;
}
