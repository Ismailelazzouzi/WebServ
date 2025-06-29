#if !defined(CLIENTSESSION_HPP)
#define CLIENTSESSION_HPP

#include "MySocket.hpp"
#include "RequestParser.hpp"
#include "ResponseBuilder.hpp"
#include "ListeningSocket.hpp"

class ClientSession
{
private:
    int listenFd;
    std::vector<pollfd> fds;
    RequestParser rp;
    ListeningSocket ls;
public:
    ClientSession(ListeningSocket ls);
    ~ClientSession();
    void    run();
    RequestParser getRp();
};


#endif