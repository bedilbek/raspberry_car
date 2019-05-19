#include "obstacle_avoidance.h"

void turn(Turn turn, int sec);

void moveUntilObstacle();

void moveUntilBlack();

bool moveUntilObstacleOrBlack();

void runIR();

void runParking(bool isleft);

using namespace std;
IRWallDetector wallDetector_;
IRLineDetector lineDetector_;
Controller controller_;
Sonar sonar_;

int avoid_obstacle(Controller controller, Sonar sonar, IRLineDetector lineDetector, IRWallDetector wallDetector)
{
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

        turn(RIGHT, 400);

        delay(500);

        moveUntilBlack();
    }

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
        }
        else {
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

    runParking(isleft);

    return 0;
}


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
			if (dis > 20 && detectorCounter >= 1 && !currentDetector) {
				controller_.stop();
				delay(sec);
				break;
			}
			lastDetector = currentDetector;
		}
	}
	else {
		lastDetector = wallDetector_.left_detected();
		controller_.turn(-100, 80);
		while (true) {
			double dis = sonar_.distance(30000);
			int currentDetector = wallDetector_.left_detected();
			if (currentDetector != lastDetector) {
				detectorCounter++;
			}
			if (dis > 20 && detectorCounter >= 1 && !currentDetector) {
				controller_.stop();
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

void runIR() {
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
        } else if (lineDetector.left_detected() && lineDetector.right_detected()) {
            break;
        } else {
            controller.forward(70);
        }
    }
    controller.stop();

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
        } else if (lineDetector.left_detected() && lineDetector.right_detected()) {
            controller.stop();
            break;
        } else {
            controller.forward(70);
        }
    }
}

void runParking(bool isleft) {
    controller.forward(60);
    delay(200);

    if (isleft) {
        controller.left(80);
    } else {
        controller.right(80);
    }

    int start = millis();
    while (true) {
        int end = millis();
        int diff = end - start;
        controller.stop();
        if (isleft && lineDetector.left_detected() && !lineDetector.right_detected()) {
            controller.forward(60);
        } else if (!isleft && lineDetector.right_detected() && !lineDetector.left_detected()) {
            controller.forward(60);
        } else if (!isleft && !lineDetector.right_detected() && !lineDetector.left_detected()) {
            controller.right(60);
        } else if (isleft && !lineDetector.right_detected() && !lineDetector.left_detected()) {
            controller.left(60);
        } else if (diff > 3000) {
            break;
        }
    }
    controller.stop();

}


bool moveUntilObstacleOrBlack() {
    bool isAfterBlack = false;
    controller.forward(70);
    while (true) {
        double dis = sonar.distance(30000);
        if (dis < 15 || wallDetector.right_detected() || wallDetector.left_detected()) {
            controller.stop();
            break;
        }
        if (lineDetector.right_detected()) {
            isAfterBlack = true;
            controller.stop();
            break;
        }
    }
    return isAfterBlack;
}