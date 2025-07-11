#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include "MySocket.hpp"

class ListeningSocket : public MySocket
{
private:
    int backLog;
public:
    ListeningSocket(int domain, int service, int protocol, int port, u_long interface,
        int bklg);
    int BindSocket();
    int StartListening();
    int acceptClient();
};

#endif