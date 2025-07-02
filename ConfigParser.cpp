#include "ConfigParser.hpp"

ConfigParser::ConfigParser()
{
}

ConfigParser::~ConfigParser()
{
}

std::string trim(const std::string& str) {
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start])) {
        ++start;
    }

    size_t end = str.length();
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }

    return str.substr(start, end - start);
}

void    ConfigParser::parse(const std::string &filepath)
{
    std::ifstream file;
    file.open(filepath);
    if (!file.is_open())
    {
        perror("Failed to open configuration file");
        exit(EXIT_FAILURE);
    }
    std::string line;
    bool    insideBlock = false;
    ServerConfig current;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
        continue;
        if (line == "server {")
        {
            insideBlock = true;
            current = ServerConfig();
        }
        else if (line == "}")
        {
            insideBlock = false;
            servers.push_back(current);
        }
        else if (insideBlock == true)
        {
            size_t spacePos = line.find(' ');
            if (spacePos == std::string::npos)
                continue;
            std::string directive = line.substr(0, spacePos);
            if (directive == "listen" || directive == "root"
                    || directive == "index" || directive == "autoindex" || directive == "error_page")
            {
                std::string value = trim(line.substr(line.find(' ') + 1));
                if (!value.empty() && value.back() == ';')
                    value.pop_back();
                if (directive == "listen")
                    current.port = std::stoi(value);
                else if (directive == "root")
                    current.root = value;
                else if (directive == "index")
                    current.index = value;
                else if (directive == "autoindex")
                {
                    if (value == "on")
                        current.autoindex = 1;
                    else
                        current.autoindex = 0;
                }
                else if (directive == "error_page")
                {
                    size_t spacePos2 = line.find(' ', spacePos + 1);
                    if (spacePos2 != std::string::npos)
                    {
                        int code = std::atoi(line.substr(spacePos + 1, spacePos2 - spacePos -1).c_str());
                        std::string path = trim(line.substr(spacePos2 + 1));
                        if (!path.empty() && path.back() == ';')
                            path.pop_back();
                        current.errorPages[code] = path;
                    }
                }
            }
            else
                std::cerr << "Warning: unknown directive: " << directive << std::endl;
        }
    }
    for (size_t i = 0; i < servers.size(); i++)
    { 
        if (servers[i].port == 0 || servers[i].root.empty() || servers[i].index.empty())
        {
            std::cerr << "Error: missing required config values on server block " << i << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

const std::vector<ServerConfig> &ConfigParser::getServers() const
{
    return servers;
}