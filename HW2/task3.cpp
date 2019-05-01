#include <wiringPi.h>
#include <iostream>
#include "../ir_line_detector.h"
#include "../controller.h"
#include "../libSonar.h"
#include <time.h>

using namespace std;

int main()
{
	if (wiringPiSetup() == -1)
	{
		return 0;
	}

	Sonar sonar;
	sonar.init(28, 29);
	IRLineDetector ir;
	Controller controller;
	controller.init_dc_motor();
	int count = 0;
	bool prev_state, current_state = false;
	cout << "Start" << endl;
	//controller.stop();
	int speed = 40, turnspeed = 45;
	controller.forward(20);
	while (true)
	{
		delay(50);
		current_state = ir.left_detected() && ir.right_detected();
		//cout << "Current: " << current_state << "  Prev: " << prev_state << endl;

		if (sonar.distance(30000) < 10)
		{
			controller.stop();
		}
		else if (!prev_state && current_state)
		{
			if (count < 3)
			{
				cout << "count < 3" << endl;
				if (ir.left_detected() && ir.right_detected())
					count++;
			}
			else
			{
				cout << "Stop" << endl;
				controller.stop();
				delay(100);
				break;
			}
			controller.forward(speed);
		}
		else if (current_state)
		{
			//cout << "Forward" << endl;
			controller.forward(speed);
		}
		else if (ir.left_detected() && !ir.right_detected())
		{
			//cout << "Left" << endl;
			controller.left(turnspeed);
		}
		else if (ir.right_detected() && !ir.left_detected())
		{
			//cout << "Right" << endl;
			controller.right(turnspeed);
		}
		else
		{
			//cout << "Forward" << endl;
			controller.forward(speed);
		}

		prev_state = current_state;
	}
	cout << "End" << endl;
	return 0;
}