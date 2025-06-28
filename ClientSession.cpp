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

        rp = RequestParser((std::string)buffer);
        ResponseBuilder rb(rp);
        std::string response;
        response = rb.getToSend();
        std::cout << "Client says:\n" << std::string(buffer, bytes) << std::endl;
        bytes = send(clientFd, response.c_str(), response.length(), 0);
        if (bytes <= 0)
        {
            perror("FAILED TO SEND");
            close(clientFd);
            exit(EXIT_FAILURE);
        }
    }
}

RequestParser ClientSession::getRp()
{
    return rp;
}

ClientSession::~ClientSession()
{
    close(clientFd);
    std::cout << "Closed client socket: " << clientFd << std::endl;
}