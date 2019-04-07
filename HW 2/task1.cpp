#include <wiringPi.h>
#include <iostream>
#include "../ir_wall_detector.h"
#include "../controller.h"

using namespace std;

int main()
{
	if (wiringPiSetup() == -1)
	{
		return 0;
	}

	IRWallDetector ir;
	Controller controller;
	controller.init_dc_motor();

	cout << "Start" << endl;
	while (true)
	{
		if (ir.left_detected())
		{
			controller.right(80);
		}
		else if (ir.right_detected())
		{
			controller.left(80);
		}
		else
		{
			controller.forward(80);
		}
	}
	cout << "End" << endl;
	return 0;
}