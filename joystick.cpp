#include <cstdio>
#include <wiringPi.h>
#include <iostream>
#include <istream>
#include <ctime>
#include "controller.h"

using namespace std;

int main() {
    if (wiringPiSetup() == -1)
        return 0;


    Controller controller;
    controller.set_pin_mode();
    controller.init_dc_motor();

    while (true) {
        char command;
        cin >> command;
        if (command == 'a') {
            controller.left(20);
        } else if (command == 'w') {
            controller.forward(20);
        } else if (command == 's') {
            controller.backward(20);
        } else if (command == 'd') {
            controller.right(20);
        } else {
            break;
        }
    }
    controller.stop();
    return 0;
}

