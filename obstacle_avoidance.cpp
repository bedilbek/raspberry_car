#include <cstdio>
#include <iostream>
#include "ir_wall_detector.h"
#include "controller.h"
#include "libSonar.h"
#include <ctime>
#include <chrono>
#include "ir_line_detector.h"
#include <wiringPi.h>

//
// Created by Sardor on 2019-05-08.
//

using namespace std;
//bool breakIrLine = false;
//bool isLeftLineDetected = false;
//bool isRightLineDetected = false;
//IRLineDetector irLineDetector;

//PI_THREAD(startDetectingLine) {
//    while (true) {
//        if (breakIrLine)
//            break;
//        isLeftLineDetected = irLineDetector.left_detected();
//        isRightLineDetected = irLineDetector.right_detected();
//        cout << "thread" << endl;
//    }
//    return (0);
//}

int main() {
    if (wiringPiSetup() == -1) {
        cout << "wiringPi is not set up properly" << endl;
        return -1;
    }
    bool left = false;

    IRWallDetector wallDetector;
    Controller controller;
    Sonar sonar;
    controller.init_dc_motor();
    sonar.init(28, 29);
    controller.forward(70);

//    int t = piThreadCreate();
//    if (t != 0){
//        cout << "thread did not start";
//        return -1;
//    }
//    while (true) {
//        cout<<"left: " <<isLeftLineDetected <<" right: "<<isRightLineDetected<<endl;
//        if (isLeftLineDetected || isRightLineDetected)
//        {
//            breakIrLine = true;
//            cout << "finished" << endl;
//            break;
//        }
//    }
//    controller.stop();
    return 0;
}

