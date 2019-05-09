/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

#include "datagram_socket_server.h"
#include <iostream>

using namespace std;

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