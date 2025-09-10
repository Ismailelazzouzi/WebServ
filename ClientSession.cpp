#include "ClientSession.hpp"
#include <arpa/inet.h>

ClientSession::ClientSession(ConfigParser &cp) : cp(cp)
{
    for (size_t i = 0; i < cp.getServers().size(); i++)
    {
        
        int listenFd;
        pollfd serverPoll;
        ListeningSocket ls;
        for (size_t j = 0; j < cp.getServers()[i].ports.size(); j++)
        {
            ls = ListeningSocket(AF_INET, SOCK_STREAM, 0, cp.getServers()[i].ports[j], cp.getServers()[i].hostname, 10);
            lss[ls.getSock()] = ls;
            listenFd = ls.getSock();
            serverPoll.fd = listenFd;
            serverPoll.events = POLLIN;
            listenFds.push_back(listenFd);
            fds.push_back(serverPoll);
            configByFd[listenFd] = &this->cp.getServers()[i];
        }
    }
}

bool    fullRequest(std::string &request)
{
    size_t endOfHeaders = request.find("\r\n\r\n");
    if (endOfHeaders == std::string::npos)
        return false;
    std::string headers = request.substr(0, endOfHeaders);
    std::string body = request.substr(endOfHeaders + 4);

    std::string firstLine = headers.substr(0, headers.find("\n"));
    if (firstLine.find("GET") != std::string::npos || firstLine.find("DELETE") != std::string::npos)
        return true;
    else if (firstLine.find("POST") != std::string::npos)
    {
        if (headers.find("Content-Length:") != std::string::npos)
        {
            size_t pos = headers.find("Content-Length:") + 15;
            std::string lenStr = headers.substr(pos, headers.find("\r\n", pos) - pos);
            try {
                size_t cntLen = std::stoul(lenStr);
                if (body.size() >= cntLen)
                    return true;
            } catch (const std::exception& e) {
                return false;
            }
        }
        else if (headers.find("Transfer-Encoding: chunked") != std::string::npos)
        {
            if (body.find("0\r\n\r\n") != std::string::npos)
                return true;
        }
    }
    return false;
}

LocationConfig *findLocationConfig(ServerConfig *config, const std::string &uri)
{
    LocationConfig *bestMatch = nullptr;
    size_t maxLen = 0;
    for (size_t i = 0; i < config->locations.size(); i++)
    {
        if (uri.rfind(config->locations[i].path, 0) == 0)
        {
            if (config->locations[i].path.length() > maxLen)
            {
                maxLen = config->locations[i].path.length();
                bestMatch = &config->locations[i];
            }
        }
    }
    std::string str;
    str = "/" + config->index;
    if (bestMatch && bestMatch->path.compare("/") == 0 && (uri.compare(str) != 0))
        return nullptr;
    return bestMatch;
}

void    ClientSession::run()
{
    std::map<int , ClientInfo> clients;
    while (true)
    {
        if (poll(fds.data(), fds.size(), -1) <= 0)
            continue ;
        for (size_t i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                int localPort;
                int listener = fds[i].fd;
                struct sockaddr_in sin;
                socklen_t len = sizeof(sin);
                if (getsockname(listener, (struct sockaddr *)&sin, &len) == -1)
                {
                    perror("getsockname");
                    continue ;
                }
                else
                    localPort = ntohs(sin.sin_port);
                if (std::find(listenFds.begin(), listenFds.end(), fds[i].fd) != listenFds.end())
                {
                    int clientFd = lss.at(fds[i].fd).acceptClient();
                    if (clientFd < 0)
                        continue;
                    struct sockaddr_storage client_addr_storage;
                    socklen_t client_addr_len = sizeof(client_addr_storage);
                    std::string clientIp;
                    std::string clientPort;
                    if (getpeername(clientFd, (sockaddr *)&client_addr_storage, &client_addr_len) == 0)
                    { 
                        char buffer[INET6_ADDRSTRLEN];
                        if (client_addr_storage.ss_family == AF_INET)
                        {
                            struct sockaddr_in *s = (struct sockaddr_in *)&client_addr_storage;
                            inet_ntop(AF_INET, &s->sin_addr, buffer, sizeof(buffer));
                            clientIp = buffer;
                            clientPort = std::to_string(ntohs(s->sin_port));
                        }
                        else if (client_addr_storage.ss_family == AF_INET6)
                        {
                            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&client_addr_storage;
                            inet_ntop(AF_INET6, &s->sin6_addr, buffer, sizeof(buffer));
                            clientIp = buffer;
                            clientPort = std::to_string(ntohs(s->sin6_port));
                        }
                    }
                    pollfd newPollfd;
                    ClientInfo client;
                    newPollfd.fd = clientFd;
                    newPollfd.events = POLLIN;
                    fds.push_back(newPollfd);
                    client.fd = clientFd;
                    client.config = *(configByFd.at(listener));
                    client.remoteIp = clientIp;
                    client.remotePort = clientPort;
                    client.localPort = std::to_string(localPort);
                    clients[clientFd] = client;
                }
                else
                    continue;
            }        
        }
        for (int i = static_cast<int>(fds.size()) - 1; i >= (int)listenFds.size(); i--)
        {
            char buffer[1024];
            int clientFd = fds[i].fd;
            int bytes;
            if (fds[i].revents & POLLIN)
            {
                bytes = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytes <= 0)
                {
                    int clientFd = fds[i].fd;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    clients.erase(clientFd);
                    continue ;
                }
                else
                {
                    buffer[bytes] = '\0';
                    std::string request(buffer, bytes);
                    clients[fds[i].fd].requestBuffer += request;
                    if (fullRequest(clients[fds[i].fd].requestBuffer))
                    {
                        clients[fds[i].fd].rp = RequestParser(clients[fds[i].fd].requestBuffer, &clients[fds[i].fd].config);
                        clients[fds[i].fd].requestBuffer.clear();
                        LocationConfig *location = findLocationConfig(&clients[fds[i].fd].config , clients[fds[i].fd].rp.getPath());
                        clients[fds[i].fd].rb = ResponseBuilder(&clients[fds[i].fd].rp, clients[fds[i].fd].config, clients[fds[i].fd], location);
                        clients[fds[i].fd].responseBuffer = clients[fds[i].fd].rb.getToSend();
                        fds[i].events = POLLOUT;
                    }
                    else
                        continue ;
                }
            }
            if (fds[i].revents & POLLOUT)
            {
                clients[fds[i].fd].byteSent = send(fds[i].fd, 
                    clients[fds[i].fd].responseBuffer.c_str(), clients[fds[i].fd].responseBuffer.length(), 0);
                if (clients[fds[i].fd].byteSent <= 0)
                {
                    perror("FAILED TO SEND");
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    clients.erase(fds[i].fd);
                    continue ;
                }
                if (clients[fds[i].fd].byteSent == clients[fds[i].fd].responseBuffer.length())
                    fds[i].events = POLLIN;
                else if (clients[fds[i].fd].byteSent < clients[fds[i].fd].responseBuffer.length())
                    clients[fds[i].fd].responseBuffer = clients[fds[i].fd].responseBuffer.substr(clients[fds[i].fd].byteSent);
            }
        }
    }
}


RequestParser ClientSession::getRp()
{
    return rp;
}

ClientSession::~ClientSession()
{
    for (size_t i = 0; i < listenFds.size(); i++)
    {
        close(listenFds[i]);
        std::cout << "Closed client socket: " << listenFds[i] << std::endl;
    }
}