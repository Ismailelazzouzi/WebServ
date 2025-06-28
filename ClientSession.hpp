#if !defined(CLIENTSESSION_HPP)
#define CLIENTSESSION_HPP

#include "MySocket.hpp"
#include "RequestParser.hpp"
#include "ResponseBuilder.hpp"

class ClientSession
{
private:
    int clientFd;
    RequestParser rp;
public:
    ClientSession(int fd);
    ~ClientSession();
    void    handle();
    RequestParser getRp();
};


#endif