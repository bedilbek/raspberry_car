//
// Created by bedilbek on 4/6/19.
//

#ifndef SMART_CAR_IR_WALL_DETECT_H
#define SMART_CAR_IR_WALL_DETECT_H

#define IR_WALL_LEFT    26
#define IR_WALL_RIGHT   27

class IRWallDetector {
public:
    IRWallDetector();
    bool right_detected();
    bool left_detected();
    void init();
};


#endif //SMART_CAR_IR_WALL_DETECT_H
