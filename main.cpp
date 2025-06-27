#include "network.hpp"

int main(int ac, char **av)
{

    int client_fd, bytes;
    char buffer[1024];
    std::cout << "Starting...!" << std::endl;
    std::cout << "Creating Listening Socket" << std::endl;
    ListeningSocket ls = ListeningSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10);
    client_fd = ls.acceptClient();
    ls.testConnection(client_fd);
    while (true)
    {
        bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes == 0)
        {
            std::cout << "Client Disconnected!" << std::endl;
            break;
        }
        if (bytes <= 0)
        {
            perror("FAILED TO RECIEVE");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
        std::cout << "Client says: " << std::string(buffer, bytes) << std::endl;
        bytes = send(client_fd, buffer, bytes, 0);
        if (bytes <= 0)
        {
            perror("FAILED TO RECIEVE");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    }
    close(client_fd);
    std::cout << "SUCCESS!" << std::endl;
}