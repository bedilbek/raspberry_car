//
// Created by Sardor on 2019-03-11.
//

#ifndef SMART_CAR_CONTROLLER_H
#define SMART_CAR_CONTROLLER_H

class Controller {

public:

    void init_dc_motor();

    void forward(int left,int right);

    void left(int speed);

    void turn(int steering,int speed);

    void right(int speed);

    void stop();

    void backward(int left,int right);

    void go_forward_for_5();

    void turn_left_go_forward_for_3_stop();

    void turn_right_go_back_for_3_stop();

    void go_forward_for_2_left_point_stop();

    void go_backward_for_2_right_smooth_stop();

    void right_smooth(int speed);

};


/*
 *
 *
 *
 *
 *
 *
 *
 *     Controller controller;
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
 *
 *
 *
 *
 * */
#endif //SMART_CAR_CONTROLLER_H