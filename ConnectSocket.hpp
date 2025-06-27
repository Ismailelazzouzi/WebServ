#ifndef CONNECTSOCKET_HPP
#define CONNECTSOCKET_HPP

#include "MySocket.hpp"

class ConnectSocket: public MySocket
{
public:
    ConnectSocket(int domain, int service, int protocol, int port, u_long interface);
    int connectToNetwork(int sock, struct sockaddr_in adress);
};

#endif