#include <wiringPi.h>
#include <iostream>
#include "../ir_line_detector.h"
#include "../controller.h"
#include <time.h>

using namespace std;

int main()
{
	if (wiringPiSetup() == -1)
	{
		return 0;
	}

	IRLineDetector ir;
	Controller controller;
	controller.init_dc_motor();
	int count = 0;
	bool prev_state, current_state = false;
	cout << "Start" << endl;
	controller.forward(20);
	while (true)
	{
		//delay(100);
		//cout << ( ir.left_detected() && ir.right_detected() ) << endl;
		current_state = ir.left_detected() && ir.right_detected();
		cout << "Current: " << current_state << "  Prev: " << prev_state << endl;
		if (!prev_state && current_state)
		{
			if (count < 1)
			{
				cout << "count < 1" << endl;
				count++;
			}
			else
			{
				cout << "Stop" << endl;
				controller.stop();
				break;
			}
		}
		prev_state = current_state;
	}
	cout << "End" << endl;
	return 0;
}