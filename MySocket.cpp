#include "MySocket.hpp"

MySocket::MySocket(int domain, int service, int protocol, int port, u_long interface)
{
    adress.sin_family = domain;
    adress.sin_port = htons(port);
    adress.sin_addr.s_addr = htonl(interface);
    sock = socket(domain, service, protocol);
    testConnection(sock);
}
void    MySocket::testConnection(int item)
{
    if (item < 0)
    {
        perror("Connection Failed...");
        exit(EXIT_FAILURE);
    }
}

const struct sockaddr_in &MySocket::getAdress() const
{
    return this->adress;
}

int MySocket::getSock() const
{
    return this->sock;
}
