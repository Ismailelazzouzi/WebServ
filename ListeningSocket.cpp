#include "ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port,
    u_long interface, int bklg) : MySocket(domain, service, protocol, port, interface), backLog(bklg)
{
    testConnection(BindSocket());
    testConnection(StartListening());
}
int ListeningSocket::BindSocket()
{
    return bind(getSock(), (struct sockaddr *)&getAdress(), sizeof(getAdress()));
}

int ListeningSocket::StartListening()
{
    return listen(getSock(), backLog);
}

int ListeningSocket::acceptClient()
{
    int addrlen = sizeof(getAdress());
    return (accept(getSock(), (struct sockaddr *)&getAdress(), (socklen_t *)& addrlen));
}