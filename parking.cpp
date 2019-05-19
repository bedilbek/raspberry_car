//
// Created by Sardor on 2019-05-19.
//
#include <cstdio>
#include <iostream>
#include "ir_wall_detector.h"
#include "controller.h"
#include "libSonar.h"
#include <ctime>
#include "ir_line_detector.h"
#include <wiringPi.h>
#include <vector>

using namespace std;


int main() {

    if (wiringPiSetup() == -1) {
        cout << "wiringPi is not set up properly" << endl;
        return -1;
    }


    bool right = true;
    Controller controller;
    IRLineDetector lineDetector;

    controller.init_dc_motor();
    lineDetector.init();

    controller.forward(70);
    while (true) {
        if (lineDetector.right_detected() && !lineDetector.left_detected()) {
            controller.turn(100, 100);
            while (true) {
                if (!lineDetector.right_detected()) {
                    controller.stop();
                    break;
                }
                if (lineDetector.left_detected()) {
                    controller.stop();
                    break;
                }
            }
        } else if (lineDetector.left_detected() && !lineDetector.right_detected()) {
            controller.turn(-100, 100);
            while (true) {
                if (!lineDetector.left_detected()) {
                    controller.stop();
                    break;
                }
                if (lineDetector.right_detected()) {
                    controller.stop();
                    break;
                }
            }
        } else if (lineDetector.left_detected() && lineDetector.right_detected()) {
            break;
        } else {
            controller.forward(70);
        }
    }
    controller.stop();

    controller.backward(100, 100);
    delay(250);

    controller.turn(-100, 100);
    delay(500);


    controller.forward(70);
    while (true) {
        if (lineDetector.right_detected() && !lineDetector.left_detected()) {
            controller.turn(100, 100);
            while (true) {
                if (!lineDetector.right_detected()) {
                    controller.stop();
                    break;
                }
                if (lineDetector.left_detected()) {
                    controller.stop();
                    break;
                }
            }
        } else if (lineDetector.left_detected() && !lineDetector.right_detected()) {
            controller.turn(-100, 100);
            while (true) {
                if (!lineDetector.left_detected()) {
                    controller.stop();
                    break;
                }
                if (lineDetector.right_detected()) {
                    controller.stop();
                    break;
                }
            }
        } else if (lineDetector.left_detected() && lineDetector.right_detected()) {
            break;
        } else {
            controller.forward(70);
        }
    }
    controller.stop();

    return 0;
}