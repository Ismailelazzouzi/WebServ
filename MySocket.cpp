#include "MySocket.hpp"

MySocket::MySocket()
{}

MySocket::MySocket(int domain, int service, int protocol, int port, std::string interface)
{
    adress.sin_family = domain;
    adress.sin_port = htons(port);
    adress.sin_addr.s_addr = inet_addr(interface.c_str());
    sock = socket(domain, service, protocol);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
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
