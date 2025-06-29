#include "ClientSession.hpp"

ClientSession::ClientSession(ListeningSocket ls) : ls(ls), listenFd(ls.getSock())
{
    pollfd serverPoll;
    serverPoll.fd = listenFd;
    serverPoll.events = POLLIN;
    fds.push_back(serverPoll);
}

void    ClientSession::run()
{
    while (true)
    {
        if (poll(&fds[0], fds.size(), -1) < 0)
        {
            perror("POLL FAILED");
            exit(EXIT_FAILURE);
        }
        if (fds[0].revents & POLLIN)
        {
            int listener = ls.acceptClient();
            pollfd newPollfd;
            newPollfd.fd = listener;
            newPollfd.events = POLLIN;
            fds.push_back(newPollfd);
        }
        for (int i = static_cast<int>(fds.size()) - 1; i >= 1; i--)
        {
            if (!(fds[i].revents & POLLIN))
                continue;
            char buffer[1024];
            int bytes = recv(fds[i].fd, buffer, sizeof(buffer), 0);
            if (bytes <= 0)
            {
                std::cout << "Client " << i << " Disconnected!" << std::endl;
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                continue ;
            }
            else
            {
                std::string request(buffer, bytes);
                RequestParser rp(request);
                ResponseBuilder rb(rp);
                bytes = send(fds[i].fd, rb.getToSend().c_str(), rb.getToSend().length(), 0);
                if (bytes <= 0)
                {
                    perror("FAILED TO SEND");
                    close(fds[i].fd);
                    fds.erase(fds.begin() + i);
                    continue ;
                }
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
    close(listenFd);
    std::cout << "Closed client socket: " << listenFd << std::endl;
}