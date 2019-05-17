/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

#include "datagram_socket_server.h"
#include <iostream>
#include <thread>
#include <wiringPi.h>
using namespace std;

int str = 0;
int thr = 0;

void commandUpdater()
{
	datagram_socket_server *s = new datagram_socket_server(8059, "localhost", true, true);
	char message[1024];
	cout << "hello from sqldeveloper" << endl;
	while (1)
	{
		s->receive(message, 1024);
		thr = (int)message[0] == 0 ? ((int)message[1]) * -1 : (int)message[1];
		str = (int)message[2] == 0 ? ((int)message[3]) : (int)message[3] * -1;
	}
}


int main()
{
	datagram_socket_server *s = new datagram_socket_server(8059, "localhost", true, true);

	char message[1024];
	cout << "hello from sqldeveloper" << endl;
	while (1)
	{
		s->receive(message, 1024);
//        int speed = (int)message[0];
//        int steering = (int)message[1];
		cout << message <<  endl;
	}
	return 0;
}