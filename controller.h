//
// Created by Sardor on 2019-03-11.
//

#ifndef SMART_CAR_CONTROLLER_H
#define SMART_CAR_CONTROLLER_H

#define MAX_SPEED 50
#define MIN_SPEED 0

class Controller {

public:
    void init_dc_motor();

    void set_pin_mode();

    void backward(int speed);

    void forward(int speed);

    void left(int speed);

    void right(int speed);

    void stop();

};


#endif //SMART_CAR_CONTROLLER_H
