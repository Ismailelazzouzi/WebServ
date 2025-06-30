#include "network.hpp"

int checkExt(std::string configFile)
{
    int i = 0;
    while (i < configFile.length())
    {
        if (configFile[i] == '.')
            break;
        i++;
    }
    if (i == configFile.length())
        return 0;
    std::string ext = configFile.substr(i, configFile.length() - i);
    if (ext != ".conf")
    {
        return 0;
    }
    return 1;
}

int main(int ac, char **av)
{
    if (ac == 2 || ac == 1)
    {
        std::string configFile;
        if (ac == 1)
            configFile = "webserv.conf";
        else
        {
            configFile = av[1];
            if (!checkExt(configFile))
            {
                perror("wrong config file type");
                exit(EXIT_FAILURE);
            }
        }
        ConfigParser parser;
        parser.parse(configFile);
        const std::vector<ServerConfig> &configs = parser.getServers();
        for (size_t i = 0; i < configs.size(); ++i)
        {
            std::cout << "Server #" << i << ":\n";
            std::cout << "  Port:  " << configs[i].port << "\n";
            std::cout << "  Root:  " << configs[i].root << "\n";
            std::cout << "  Index: " << configs[i].index << "\n";
        }
        ClientSession cl = ClientSession(parser);
        cl.run();
    }



    

    // int listen_fd, bytes;
    // char buffer[1024];
    // std::cout << "Starting...!" << std::endl;
    // std::cout << "Creating Listening Socket" << std::endl;
    // ListeningSocket ls = ListeningSocket(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
    // std::cout << "SUCCESS!" << std::endl;
}