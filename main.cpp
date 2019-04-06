#include <cstdio>
#include <wiringPi.h>
#include <iostream>
#include <istream>
#include <ctime>
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <zconf.h>
#include "libSonar.h"
#include "controller.h"

using namespace std;

int main() {
    if (wiringPiSetup() == -1) {
        return 0;
    }

    Controller controller;
    controller.init_dc_motor();
    controller.forward(80);
    delay(1000);
    controller.backward(80);
    delay(1000);
    controller.stop();

    return 0;
}

