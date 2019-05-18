#include <cstdio>
#include <iostream>
#include "ir_wall_detector.h"
#include "controller.h"
#include "libSonar.h"
#include <ctime>
#include "ir_line_detector.h"
#include <wiringPi.h>

//
// Created by Sardor on 2019-05-08.
//

using namespace std;
IRWallDetector wallDetector;
IRLineDetector lineDetector;
Controller controller;
Sonar sonar;


enum Turn {
    LEFT,
    RIGHT
};

void turn(Turn turn, int sec);

void moveUntilObstacle();

void moveUntilBlack();

void runIr();

int main() {

    if (wiringPiSetup() == -1) {
        cout << "wiringPi is not set up properly" << endl;
        return -1;
    }

    controller.init_dc_motor();
    lineDetector.init();
    wallDetector.init();
    sonar.init(24, 25);

    moveUntilObstacle();

    delay(500);
    //step (2) turn car until wall detects nothing
    turn(RIGHT, 400);

    delay(500);

    moveUntilBlack();


    //---------------------------------
    controller.turn(100, 100);
    delay(200);

    while (true) {
        double dis = sonar.distance(30000);
        if (lineDetector.right_detected()) {
            controller.turn(100, 100);
            while (true) {
                if (!lineDetector.right_detected()) {
                    controller.stop();
                    break;
                }
            }
        } else {
            controller.forward(50);
        }
        if (dis < 20) {
            controller.turn(100, 80);
            delay(100);
            controller.stop();
            break;
        }
    }

    turn(LEFT, 0);

    moveUntilObstacle();
    turn(RIGHT, 0);

    runIr();


    return 0;
}

void turn(Turn turn, int sec) {
    int lastDetector = 0;
    int detectorCounter = 0;
    if (turn == LEFT) {
        lastDetector = wallDetector.right_detected();
        controller.turn(100, 80);
        while (true) {
            double dis = sonar.distance(30000);
            int currentDetector = wallDetector.right_detected();
            cout << currentDetector << endl;
            if (currentDetector != lastDetector) {
                detectorCounter++;

            }
            if (dis > 20 && detectorCounter >= 2 && !currentDetector) {
                controller.stop();
                delay(sec);
                break;
            }
            lastDetector = currentDetector;
        }
    } else {
        lastDetector = wallDetector.left_detected();
        controller.turn(-100, 80);
        while (true) {
            double dis = sonar.distance(30000);
            int currentDetector = wallDetector.left_detected();
            if (currentDetector != lastDetector) {
                detectorCounter++;
            }
            if (dis > 20 && detectorCounter >= 2 && !currentDetector) {
                controller.stop();
                break;
            }
            lastDetector = currentDetector;
        }
    }
}

void moveUntilObstacle() {
    controller.forward(70);
    while (true) {
        double dis = sonar.distance(30000);
        if (dis < 20) {
            controller.stop();
            break;
        }
    }
}

void moveUntilBlack() {
    controller.forward(70);
    while (true) {
        if (lineDetector.right_detected()) {
            controller.stop();
            break;
        }
    }
}

void runIr() {
    controller.forward(70);
    while (true) {
        if (lineDetector.right_detected()) {
            controller.turn(100, 100);
            while (true) {
                if (!lineDetector.right_detected()) {
                    controller.stop();
                    break;
                }
            }
        } else if (lineDetector.left_detected()) {
            controller.turn(-100, 100);
            while (true) {
                if (!lineDetector.left_detected()) {
                    controller.stop();
                    break;
                }
            }
        } else {
            controller.forward(70);
        }
    }
}
