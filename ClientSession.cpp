#include "ClientSession.hpp"

ClientSession::ClientSession(ConfigParser cp) : cp(cp)
{
    for (size_t i = 0; i < cp.getServers().size(); i++)
    {
        
        int listenFd;
        pollfd serverPoll;
        lss.push_back(ListeningSocket(AF_INET, SOCK_STREAM, 0, cp.getServers()[i].port, INADDR_ANY, 10));
        listenFd = lss[i].getSock();
        serverPoll.fd = listenFd;
        serverPoll.events = POLLIN;
        listenFds.push_back(listenFd);
        fds.push_back(serverPoll);
    }
}

bool    fullRequest(std::string &request)
{
    size_t endOfHeaders = request.find("\r\n\r\n");
    if (endOfHeaders == std::string::npos)
        return false;
    std::string headers = request.substr(0, endOfHeaders);
    std::string body = request.substr(endOfHeaders + 4);
    if (headers.substr(0, headers.find("\n")).find("GET") != std::string::npos)
        return true;
    else if (headers.find("Transfer-Encoding: chunked") != std::string::npos 
        && body.find("0\r\n\r\n") == std::string::npos)
        return false;
    else if (headers.find("Transfer-Encoding: chunked") != std::string::npos 
        && body.find("0\r\n\r\n") != std::string::npos)
        return true;
    else if (headers.find("Content-Length") != std::string::npos)
    {
        std::string contentLen = headers.substr(headers.find("Content-Length:") + 15);
        contentLen = contentLen.substr(0, contentLen.find("\r\n"));
        size_t cntLen = std::stol(contentLen);
        if (body.size() < cntLen)
            return false;
        return true;
    }
    return true;
}

void    ClientSession::run()
{
    std::vector<ClientInfo> clients;
    while (true)
    {
        if (poll(fds.data(), fds.size(), -1) <= 0)
            continue ;
        for (size_t i = 0; i < listenFds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                int listener = lss[i].acceptClient();
                pollfd newPollfd;
                ClientInfo client;
                newPollfd.fd = listener;
                newPollfd.events = POLLIN;
                fds.push_back(newPollfd);
                client.fd = listener;
                client.config = cp.getServers()[i];
                clients.push_back(client);
            }        
        }
        for (int i = static_cast<int>(fds.size()) - 1; i >= (int)listenFds.size(); i--)
        {
            char buffer[1024];
            int clientFd = fds[i].fd;
            int clientPos = currentClientPos(clients, clientFd);
            int bytes;
            if (fds[i].revents & POLLIN)
            {
                bytes = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytes <= 0)
                {
                    int clientFd = fds[i].fd;
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    for (size_t j = 0; j < clients.size(); ++j)
                    {
                        if (clients[j].fd == clientFd)
                        {
                            clients.erase(clients.begin() + j);
                            break;
                        }
                    }
                    continue ;
                }
                else
                {
                    buffer[bytes] = '\0';
                    std::string request(buffer, bytes);
                    clients[clientPos].requestBuffer += request; // request buffer to hold chunked requests
                    if (fullRequest(clients[clientPos].requestBuffer))
                    {
                        clients[clientPos].rp = RequestParser(clients[clientPos].requestBuffer, clients[clientPos].config);
                        clients[clientPos].requestBuffer.clear();
                        fds[i].events = POLLOUT;
                    }
                    else
                        continue ;
                }
            }
            if (fds[i].revents & POLLOUT)
            {
                clients[clientPos].rb = ResponseBuilder(clients[clientPos].rp, clients[clientPos].config.root);
                bytes = send(fds[i].fd, clients[clientPos].rb.getToSend().c_str(), clients[clientPos].rb.getToSend().length(), 0);
                if (bytes <= 0)
                {
                    perror("FAILED TO SEND");
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    continue ;
                }
                fds[i].events = POLLIN;
            }
        }
        
    }
}

int ClientSession::currentClientPos(const std::vector<ClientInfo> &clients, int clientFd)
{
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].fd == clientFd)
            return i;
    }
    return (-1);    
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