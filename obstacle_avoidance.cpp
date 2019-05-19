#include "obstacle_avoidance.h"
#include "pthread.h"

using namespace std;
IRWallDetector wallDetector_;
IRLineDetector lineDetector_;
Controller controller_;
Sonar sonar_;

bool left_line = false;
bool right_line = false;
bool stopIrLineThread = false;

void runParking();

void runNewAlgorithm();

void *ir_thread(void *) {
    while (true) {
        if (lineDetector_.left_detected()) {
            controller_.turn(-100, 100);
            delay(200);
        } else if (lineDetector_.right_detected()) {
            controller_.turn(100, 100);
            delay(200);
        } else if (sonar_.distance(30000) < 20) {
            controller_.stop();
        } else if (stopIrLineThread) {
            break;
        } else {
            controller_.forward(60);
        }
    }
}

int avoid_obstacle(Controller controller, Sonar sonar, IRLineDetector lineDetector, IRWallDetector wallDetector) {
    if (wiringPiSetup() == -1) {
        cout << "wiringPi is not set up properly" << endl;
        return -1;
    }
    controller_ = controller;
    wallDetector_ = wallDetector;
    lineDetector_ = lineDetector;
    sonar_ = sonar;

    bool isleft = true;

    bool result = moveUntilObstacleOrBlack();

    if (!result) {

        delay(500);

        turn(LEFT);

        delay(500);

        moveUntilBlack();
    }

    //---------------------------------
    controller_.turn(100, 100);
    delay(200);

    while (true) {
        double dis = sonar_.distance(30000);
        if (lineDetector_.right_detected()) {
            controller_.turn(100, 100);
            while (true) {
                if (!lineDetector_.right_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else {
            controller_.forward(50);
        }
        if (dis < 20) {
            controller_.turn(100, 80);
            delay(100);
            controller_.stop();
            break;
        }
    }
//
//    turn(LEFT, 0);
//
//    moveUntilObstacle(20);
//    turn(RIGHT, 0);

    runParking(isleft);

    return 0;
}

Turn LASTCOMMAND = LEFT;

Turn nextCommand(Turn turn);

void runNewDynamicAlgorithm();


int main() {

    if (wiringPiSetup() == -1) {
        cout << "wiringPi is not set up properly" << endl;
        return -1;
    }
    controller_.init_dc_motor();
    lineDetector_.init();
    wallDetector_.init();
    sonar_.init(24, 25);
    moveUntilObstacle(15);

    turn(LEFT);

    bool res = moveUntilObstacleOrBlack();
    if (!res) {
        controller_.turn(100, 100);
        delay(200);
        while (true) {
            double dis = sonar_.distance(30000);
            if (lineDetector_.left_detected()) {
                controller_.turn(100, 100);
                while (true) {
                    if (!lineDetector_.left_detected()) {
                        controller_.stop();
                        break;
                    }
                }
            } else {
                controller_.forward(60);
            }
            if (dis < 20) {
                controller_.turn(100, 80);
                delay(100);
                controller_.stop();
                break;
            }
        }
        turn(RIGHT);
        moveUntilObstacle(10);
        turn(LEFT);
    } else {
        moveUntilObstacle(10);
        turn(RIGHT);
    }
    runParking();
    return 0;
}

bool turn(Turn turn) {
    bool isBlackDetected = false;

    int lastDetector = 0;
    int detectorCounter = 0;
    if (turn == LEFT) {
        lastDetector = wallDetector_.right_detected();
        controller_.turn(100, 100);
        while (true) {
            double dis = sonar_.distance(30000);
            int currentDetector = wallDetector_.right_detected();
            if (lineDetector_.left_detected()) {
                isBlackDetected = true;
                controller_.stop();
                break;
            }
            if (currentDetector != lastDetector) {
                detectorCounter++;
            }
            if (dis > 20 && detectorCounter >= 1 && !currentDetector) {
                delay(200);
                controller_.stop();
                break;
            }
            lastDetector = currentDetector;
        }
    } else {
        lastDetector = wallDetector_.left_detected();
        controller_.turn(-100, 100);
        while (true) {
            double dis = sonar_.distance(30000);
            int currentDetector = wallDetector_.left_detected();
            if (lineDetector_.right_detected()) {
                isBlackDetected = true;
                controller_.stop();
                break;
            }
            if (currentDetector != lastDetector) {
                detectorCounter++;
            }
            if (dis > 20 && detectorCounter >= 1 && !currentDetector) {
                delay(200);
                controller_.stop();
                break;
            }
            lastDetector = currentDetector;
        }
    }
    return isBlackDetected;
}

void moveUntilObstacle(int limit) {
    controller_.forward(70);
    while (true) {
        double dis = sonar_.distance(30000);
        if (dis < limit) {
            controller_.stop();
            break;
        }
    }
}

void moveUntilBlack() {
    controller_.forward(70);
    while (true) {
        if (lineDetector_.right_detected()) {
            controller_.stop();
            break;
        }
    }
}

void runIR() {
    controller_.forward(70);
    while (true) {
        if (lineDetector_.right_detected()) {
            controller_.turn(100, 100);
            while (true) {
                if (!lineDetector_.right_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else if (lineDetector_.left_detected()) {
            controller_.turn(-100, 100);
            while (true) {
                if (!lineDetector_.left_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else if (lineDetector_.left_detected() && lineDetector_.right_detected()) {
            break;
        } else {
            controller_.forward(70);
        }
    }
    controller_.stop();

    controller_.forward(70);
    while (true) {
        if (lineDetector_.right_detected()) {
            controller_.turn(100, 100);
            while (true) {
                if (!lineDetector_.right_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else if (lineDetector_.left_detected()) {
            controller_.turn(-100, 100);
            while (true) {
                if (!lineDetector_.left_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else if (lineDetector_.left_detected() && lineDetector_.right_detected()) {
            controller_.stop();
            break;
        } else {
            controller_.forward(70);
        }
    }
}

void runParking(bool isleft) {
    controller_.forward(60);
    delay(200);

    if (isleft) {
        controller_.left(80);
    } else {
        controller_.right(80);
    }

    int start = millis();
    while (true) {
        int end = millis();
        int diff = end - start;
        controller_.stop();
        if (isleft && lineDetector_.left_detected() && !lineDetector_.right_detected()) {
            controller_.forward(60);
        } else if (!isleft && lineDetector_.right_detected() && !lineDetector_.left_detected()) {
            controller_.forward(60);
        } else if (!isleft && !lineDetector_.right_detected() && !lineDetector_.left_detected()) {
            controller_.right(60);
        } else if (isleft && !lineDetector_.right_detected() && !lineDetector_.left_detected()) {
            controller_.left(60);
        } else if (diff > 3000) {
            break;
        }
    }
    controller_.stop();

}


bool moveUntilObstacleOrBlack() {
    bool isBlack = false;
    controller_.forward(70);
    while (true) {
        double dis = sonar_.distance(30000);
        if (dis < 15) {
            controller_.stop();
            break;
        }
        if (lineDetector_.left_detected()) {
            isBlack = true;
            controller_.stop();
            break;
        }
    }
    return isBlack;
}

void runParking() {


    controller_.forward(70);
    while (true) {
        if (lineDetector_.right_detected() && !lineDetector_.left_detected()) {
            controller_.turn(100, 100);
            while (true) {
                if (!lineDetector_.right_detected()) {
                    controller_.stop();
                    break;
                }
                if (lineDetector_.left_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else if (lineDetector_.left_detected() && !lineDetector_.right_detected()) {
            controller_.turn(-100, 100);
            while (true) {
                if (!lineDetector_.left_detected()) {
                    controller_.stop();
                    break;
                }
                if (lineDetector_.right_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else if (lineDetector_.left_detected() && lineDetector_.right_detected()) {
            break;
        } else {
            controller_.forward(70);
        }
    }
    controller_.stop();

    controller_.backward(100, 100);
    delay(250);

    controller_.turn(-100, 100);
    delay(500);


    controller_.forward(70);
    while (true) {
        if (lineDetector_.right_detected() && !lineDetector_.left_detected()) {
            controller_.turn(100, 100);
            while (true) {
                if (!lineDetector_.right_detected()) {
                    controller_.stop();
                    break;
                }
                if (lineDetector_.left_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else if (lineDetector_.left_detected() && !lineDetector_.right_detected()) {
            controller_.turn(-100, 100);
            while (true) {
                if (!lineDetector_.left_detected()) {
                    controller_.stop();
                    break;
                }
                if (lineDetector_.right_detected()) {
                    controller_.stop();
                    break;
                }
            }
        } else if (lineDetector_.left_detected() && lineDetector_.right_detected()) {
            break;
        } else {
            controller_.forward(70);
        }
    }
    controller_.backward(100, 100);
    delay(250);
    controller_.stop();
}

Turn nextCommand(Turn turn) {
    if (turn == RIGHT) {
        return LEFT;
    } else {
        return RIGHT;
    }
}