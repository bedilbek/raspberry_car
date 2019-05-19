#include "obstacle_avoidance.h"

void turn(Turn turn, int sec);

void moveUntilObstacle();

void moveUntilBlack();

bool moveUntilObstacleOrBlack();

void runIR();

void runParking(bool isleft);


//
// Created by Sardor on 2019-05-08.
//

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

void turn(Turn turn, int sec) {
	int lastDetector = 0;
	int detectorCounter = 0;
	if (turn == LEFT) {
		lastDetector = wallDetector_.right_detected();
		controller_.turn(100, 80);
		while (true) {
			double dis = sonar_.distance(30000);
			int currentDetector = wallDetector_.right_detected();
			cout << currentDetector << endl;
			if (currentDetector != lastDetector) {
				detectorCounter++;

			}
			if (dis > 20 && detectorCounter >= 2 && !currentDetector) {
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
			if (dis > 20 && detectorCounter >= 2 && !currentDetector) {
				controller_.stop();
				break;
			}
			lastDetector = currentDetector;
		}
	}
}

void moveUntilObstacle() {
	controller_.forward(70);
	while (true) {
		double dis = sonar_.distance(30000);
		if (dis < 20) {
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
		}
		else if (lineDetector_.left_detected()) {
			controller_.turn(-100, 100);
			while (true) {
				if (!lineDetector_.left_detected()) {
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
		}
		else if (lineDetector_.left_detected()) {
			controller_.turn(-100, 100);
			while (true) {
				if (!lineDetector_.left_detected()) {
					controller_.stop();
					break;
				}
			}
		}
		else if (lineDetector_.left_detected() && lineDetector_.right_detected()) {
			controller_.stop();
			break;
		}
		else {
			controller_.forward(70);
		}
	}
}

void runParking(bool isleft) {
	controller_.forward(60);
	delay(200);

	if (isleft) {
		controller_.left(80);
	}
	else {
		controller_.right(80);
	}

	int start = millis();
	while (true) {
		int end = millis();
		int diff = end - start;
		controller_.stop();
		if (isleft && lineDetector_.left_detected() && !lineDetector_.right_detected()) {
			controller_.forward(60);
		}
		else if (!isleft && lineDetector_.right_detected() && !lineDetector_.left_detected()) {
			controller_.forward(60);
		}
		else if (!isleft && !lineDetector_.right_detected() && !lineDetector_.left_detected()) {
			controller_.right(60);
		}
		else if (isleft && !lineDetector_.right_detected() && !lineDetector_.left_detected()) {
			controller_.left(60);
		}
		else if (diff > 3000) {
			break;
		}
	}
	controller_.stop();

}


bool moveUntilObstacleOrBlack() {
	bool isAfterBlack = false;
	controller_.forward(70);
	while (true) {
		double dis = sonar_.distance(30000);
		if (dis < 20) {
			controller_.stop();
			break;
		}
		if (lineDetector_.right_detected()) {
			isAfterBlack = true;
			controller_.stop();
			break;
		}
	}
	return isAfterBlack;
}