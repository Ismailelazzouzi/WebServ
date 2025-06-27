#include "ConnectSocket.hpp"

ConnectSocket::ConnectSocket(int domain, int service, int protocol, int port, u_long interface) :
    MySocket(domain, service, protocol, port, interface)
{
    testConnection(connectToNetwork(getSock(), getAdress()));
}

int ConnectSocket::connectToNetwork(int sock, struct sockaddr_in adress)
{
    return connect(sock, (struct sockaddr *)&adress, sizeof(adress));
}