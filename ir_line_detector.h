//
// Created by bedilbek on 4/6/19.
//

#ifndef SMART_CAR_IR_LINE_DETECTOR_H
#define SMART_CAR_IR_LINE_DETECTOR_H

#define IR_LINE_LEFT    10
#define IR_LINE_RIGHT   11

// from white surface receives 0
// from black surface receives 1

class IRLineDetector {
public:
    IRLineDetector();
    bool right_detected();
    bool left_detected();
    bool isLeftOnBlack();
    bool isRightOnBlack();
    void init();
};


#endif //SMART_CAR_IR_LINE_DETECTOR_H
