#include "network.hpp"

int main(int ac, char **av)
{

    int listen_fd, bytes;
    char buffer[1024];
    std::cout << "Starting...!" << std::endl;
    std::cout << "Creating Listening Socket" << std::endl;
    ListeningSocket ls = ListeningSocket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
    ClientSession cl = ClientSession(ls);
    cl.run();
    std::cout << "SUCCESS!" << std::endl;
}