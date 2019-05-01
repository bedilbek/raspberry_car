//
// Created by bedilbek on 4/24/19.
//

#ifndef SMART_CAR_DATAGRAM_SOCKET_SERVER_H
#define SMART_CAR_DATAGRAM_SOCKET_SERVER_H


#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#endif


//Simple socket class for datagrams.  Platform independent between
//unix and Windows.
class datagram_socket_server
{
private:
#ifdef WIN32
    WSAData wsaData;
    SOCKET sock;
#else
    int sock;
#endif
    long retval;
    sockaddr_in outaddr;
    char ip[30];
    char received[30];


public:
    datagram_socket_server(int port, char* address, bool Datagram, bool reusesock);
    ~datagram_socket_server();

    long receive(char* msg, int msgsize);
    char* received_from();
    long send(const char* msg, int msgsize);
    long sendTo(const char* msg, int msgsize, const char* name);
    int getAddress(const char * name, char * addr);
    const char* getAddress(const char * name);

};

#endif //SMART_CAR_DATAGRAM_SOCKET_SERVER_H
