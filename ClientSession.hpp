#if !defined(CLIENTSESSION_HPP)
#define CLIENTSESSION_HPP

#include "MySocket.hpp"
#include "RequestParser.hpp"
#include "ResponseBuilder.hpp"
#include "ListeningSocket.hpp"
#include "ConfigParser.hpp"

struct ClientInfo {
    int fd;
    ServerConfig config;
    std::string requestBuffer;
    std::string responseBuffer;
    RequestParser rp;
    ResponseBuilder rb;
};

class ClientSession
{
private:
    std::vector <int> listenFds;
    std::vector<pollfd> fds;
    RequestParser rp;
    std::vector<ListeningSocket> lss;
    ConfigParser cp;
    
public:
    ClientSession(ConfigParser cp);
    ~ClientSession();
    void    run();
    RequestParser getRp();
    int currentClientPos(const std::vector<ClientInfo> &clients, int clientFd);
};


#endif