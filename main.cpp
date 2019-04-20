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
    controller.init_dc_motor();

    while (true) {
        int steering;
        cin >> steering;
        if (steering > 0) {
            float x = 100.0 - (steering / 100.0) * 100.0;
            cout<<"x:"<<x<<endl;
            controller.turn(x, 100);
        } else {
            steering = -steering;
            float y = 100.0 - (steering / 100.0) * 100.0;
            cout<<"y:"<<y<<endl;
            controller.turn(100, y);
        }
    }
    controller.stop();
    return 0;
}

