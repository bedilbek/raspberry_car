//
// Created by Sardor on 2019-05-15.
//
#include <cstdio>
#include <iostream>
#include "controller.h"
#include <wiringPi.h>

#include "stop.h"

int main() {
    if (wiringPiSetup() == -1) {
        return -1;
    }
    Controller controller;
    controller.init_dc_motor();
    controller.stop();
}
