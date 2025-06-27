#include "ClientSession.hpp"

ClientSession::ClientSession(int fd) : clientFd(fd)
{
}

void    ClientSession::handle()
{
    char buffer[1024];
    while (true)
    {
        int bytes = recv(clientFd, buffer, sizeof(buffer), 0);
        if (bytes == 0)
        {
            std::cout << "Client Disconnected!" << std::endl;
            break;
        }
        if (bytes <= 0)
        {
            perror("FAILED TO RECIEVE");
            close(clientFd);
            exit(EXIT_FAILURE);
        }
        std::cout << "Client says: " << std::string(buffer, bytes) << std::endl;
        bytes = send(clientFd, buffer, bytes, 0);
        if (bytes <= 0)
        {
            perror("FAILED TO SEND");
            close(clientFd);
            exit(EXIT_FAILURE);
        }
    }
}
ClientSession::~ClientSession()
{
    close(clientFd);
    std::cout << "Closed client socket: " << clientFd << std::endl;
}