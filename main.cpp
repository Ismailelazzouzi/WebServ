#include "ListeningSocket.hpp"
#include "ClientSession.hpp"

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
        ClientSession cl = ClientSession(parser);
        std::cout << "SERVER IS RUNNUNG" << std::endl;
        cl.run();
    }
}