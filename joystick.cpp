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
    controller.init_dc_motor();

    while (true) {
        char command;
        cin >> command;
        if (command == 'a') {
            controller.left(80);
        } else if (command == 'w') {
            controller.forward(80);
        } else if (command == 's') {
            controller.backward(80);
        } else if (command == 'd') {
            controller.right(80);
        } else {
            break;
        }
    }
    controller.stop();
    return 0;
}

