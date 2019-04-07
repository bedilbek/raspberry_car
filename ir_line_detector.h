//
// Created by bedilbek on 4/6/19.
//

#ifndef SMART_CAR_IR_LINE_DETECTOR_H
#define SMART_CAR_IR_LINE_DETECTOR_H

#define IR_LINE_LEFT    10
#define IR_LINE_RIGHT   11

class IRLineDetector {
public:
    IRLineDetector();
    bool right_detected();
    bool left_detected();

private:
    void init();
};


#endif //SMART_CAR_IR_LINE_DETECTOR_H
