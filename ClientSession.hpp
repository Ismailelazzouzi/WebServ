#if !defined(CLIENTSESSION_HPP)
#define CLIENTSESSION_HPP

#include "MySocket.hpp"

class ClientSession
{
private:
    int clientFd;
public:
    ClientSession(int fd);
    ~ClientSession();
    void    handle();
};


#endif