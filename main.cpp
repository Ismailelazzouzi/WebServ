#include "network.hpp"

int main(int ac, char **av)
{

    int client_fd, bytes;
    char buffer[1024];
    std::cout << "Starting...!" << std::endl;
    std::cout << "Creating Listening Socket" << std::endl;
    ListeningSocket ls = ListeningSocket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
    client_fd = ls.acceptClient();
    ls.testConnection(client_fd);
    ClientSession cl = ClientSession(client_fd);
    cl.handle();
    std::cout << "SUCCESS!" << std::endl;
}