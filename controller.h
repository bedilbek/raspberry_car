//
// Created by Sardor on 2019-03-11.
//

#ifndef SMART_CAR_CONTROLLER_H
#define SMART_CAR_CONTROLLER_H




class Controller {

public:

    void init_dc_motor();

    void forward(int speed);

	void forward(int left, int right);

    void left(int speed);

    void right(int speed);

    void stop();

    void backward(int speed);

    void go_forward_for_5();

    void turn_left_go_forward_for_3_stop();

    void turn_right_go_back_for_3_stop();

    void go_forward_for_2_left_point_stop();

    void go_backward_for_2_right_smooth_stop();

    void right_smooth(int speed);


};



#endif //SMART_CAR_CONTROLLER_H
