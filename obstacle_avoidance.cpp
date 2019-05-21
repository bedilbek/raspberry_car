#include "obstacle_avoidance.h"
#include "pthread.h"

//
// Created by Sardor on 2019-05-08.
//

    if (res) {
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

int avoid_obstacle(Controller controller, Sonar sonar, IRLineDetector lineDetector, IRWallDetector wallDetector)
{
	controller_ = controller;
	wallDetector_ = wallDetector;
	lineDetector_ = lineDetector;
	sonar_ = sonar;

	moveUntilObstacle(15);

	turn_a(LEFT);

	bool res = moveUntilObstacleOrBlack();

	if (res) {
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
			}
			else {
				controller_.forward(60);
			}
			if (dis < 20) {
				controller_.turn(100, 80);
				delay(100);
				controller_.stop();
				break;
			}
		}
		turn_a(RIGHT);

		moveUntilObstacle(15);

		turn_a(LEFT);
	}
	else {
		turn_a(RIGHT);
	}
	runParking();
	return 0;
}

bool turn_a(Turn turn) {
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
				controller_.stop();
				break;
			}
			lastDetector = currentDetector;
		}
	}
	else {
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
		if (dis < limit || wallDetector_.left_detected() || wallDetector_.right_detected()) {
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

bool moveUntilObstacleOrBlack() {
	bool isBlack = false;
	controller_.forward(70);
	while (true) {
		double dis = sonar_.distance(30000);
		if (dis < 15 || wallDetector_.left_detected() || wallDetector_.right_detected()) {
			isBlack = false;
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
		}
		else if (lineDetector_.left_detected() && !lineDetector_.right_detected()) {
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
		}
		else if (lineDetector_.left_detected() && lineDetector_.right_detected()) {
			break;
		}
		else {
			controller_.forward(70);
		}
	}
	controller_.stop();

	controller_.backward(100, 100);
	delay(250);

	controller_.turn(100, 100);
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
		}
		else if (lineDetector_.left_detected() && !lineDetector_.right_detected()) {
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
		}
		else if (lineDetector_.left_detected() && lineDetector_.right_detected()) {
			break;
		}
		else {
			controller_.forward(70);
		}
	}
	controller_.backward(100, 100);
	delay(250);
	controller_.stop();
}