//
// Created by Sardor on 2019-03-11.
//

#ifndef SMART_CAR_CONTROLLER_H
#define SMART_CAR_CONTROLLER_H
#include "ir_line_detector.h"


class Controller {

public:

    void init_dc_motor();

    void forward(int left, int right);

    void left(int speed);

    void turn(int steering, int speed);

    void right(int speed);

    void stop();

    void backward(int left, int right);

    void forward(int speed);

    void turnLeft90Gradus(int speed);

    void turnRight90Gradus(int speed);

    void moveUntilItDetectsYellow(int speed,IRLineDetector irLineDetector);
};

#endif //SMART_CAR_CONTROLLER_H