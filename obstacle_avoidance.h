#ifndef OBSTC
#define OBSTC

#include <cstdio>
#include <iostream>
#include "ir_wall_detector.h"
#include "controller.h"
#include "libSonar.h"
#include <ctime>
#include "ir_line_detector.h"
#include <wiringPi.h>
#include <vector>

enum Turn
{
	LEFT,
	RIGHT
};

bool turn(Turn turn);

void moveUntilObstacle(int limit);

void moveUntilBlack();

bool moveUntilObstacleOrBlack();

void runIR();

void runParking(bool isleft);

int avoid_obstacle(Controller controller, Sonar sonar, IRLineDetector lineDetector, IRWallDetector wallDetector);

#endif // !OBSTC