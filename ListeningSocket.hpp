#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include "MySocket.hpp"

class ListeningSocket : public MySocket
{
private:
    int backLog;
public:
    ListeningSocket();
    ListeningSocket(int domain, int service, int protocol, int port, std::string interface,
        int bklg);
    int BindSocket();
    int StartListening();
    int acceptClient();
};

#endif