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
    Controller controller;
    int steering = -50;
    controller.init_dc_motor();
    controller.forward(80);
    delay(1000);
    if (steering > 0) {
        int left = 100 - (steering / 100) * 100;
        int right = 100;
        controller.turn(left, right);
    } else {
        steering = -steering;
        int right = 100 - (steering / 100) * 100;
        int left = 100;
        controller.turn(left, right);
    }
    delay(1000);
    controller.forward(80);
    delay(1000);
    controller.stop();
    return 0;
}

