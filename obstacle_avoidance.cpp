#include <cstdio>
#include <iostream>
#include "ir_wall_detector.h"
#include "controller.h"
#include "libSonar.h"
#include <wiringPi.h>
//
// Created by Sardor on 2019-05-08.
//
using namespace std;

int main() {
    if (wiringPiSetup() == -1) {
        cout << "wiringPi is not set up properly" << endl;
        return -1;
    }
    IRWallDetector wallDetector;
    Controller controller;
    Sonar sonar;
    controller.init_dc_motor();
    sonar.init(28, 29);
    while (true) {
        if (wallDetector.left_detected() == 1) {

        } else {

        }
        if (wallDetector.right_detected() == 1) {

        } else {

        }
        if (sonar.distance(300000) <= 20) {
            //obstacle in front
        } else {

        }

    }

    return 0;
}
