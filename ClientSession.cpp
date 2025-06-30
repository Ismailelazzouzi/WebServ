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
            if (!(fds[i].revents & POLLIN))
                continue;
            char buffer[1024];
            int bytes = recv(fds[i].fd, buffer, sizeof(buffer), 0);
            if (bytes <= 0)
            {
                int clientFd = fds[i].fd;
                std::cout << "Client on fd " << fds[i].fd << " disconnected!" << std::endl;
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
                int clientFd = fds[i].fd;
                ServerConfig* config = nullptr;
                for (size_t j = 0; j < clients.size(); ++j)
                {
                    if (clients[j].fd == clientFd)
                    {
                        config = &clients[j].config;
                        break;
                    }
                }
                if (!config)
                    continue;
                buffer[bytes] = '\0';
                std::string request(buffer, bytes);
                std::cout << request << std::endl;
                RequestParser rp(request, config->index);
                ResponseBuilder rb(rp, config->root);
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
    for (size_t i = 0; i < listenFds.size(); i++)
    {
        close(listenFds[i]);
        std::cout << "Closed client socket: " << listenFds[i] << std::endl;
    }
}