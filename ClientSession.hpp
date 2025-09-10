#if !defined(CLIENTSESSION_HPP)
#define CLIENTSESSION_HPP

#include "RequestParser.hpp"
#include "ResponseBuilder.hpp"
#include "ListeningSocket.hpp"

struct ClientInfo {
    int fd;
    ServerConfig config;
    std::string requestBuffer;
    std::string responseBuffer;
    std::string remotePort;
    std::string remoteIp;
    std::string localPort;
    RequestParser rp;
    ResponseBuilder rb;
    size_t byteSent;
};

class ClientSession
{
private:
    std::vector <int> listenFds;
    std::vector<pollfd> fds;
    std::map<int, const ServerConfig*> configByFd;
    RequestParser rp;
    std::map<int ,ListeningSocket> lss;
    ConfigParser cp;
    
public:
    ClientSession(ConfigParser &cp);
    ~ClientSession();
    void    run();
    RequestParser getRp();
};

#endif