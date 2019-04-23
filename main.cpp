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

    controller.stop();
    return 0;
}

