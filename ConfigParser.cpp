#include "ConfigParser.hpp"

ConfigParser::ConfigParser()
{
}

ConfigParser::~ConfigParser()
{
}

std::string trim(const std::string& str)
{
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

size_t extractSize(std::string &value)
{
    size_t size = 1048576;
    std::string ext;
    std::string num;
    for (size_t i = 0; i < value.length(); i++)
    {
        if (!(value[i] >= '0' && value[i] <= '9'))
        {
            ext = value.substr(i);
            num = value.substr(0, i);
            break;
        }
    }
    if (ext == "k" || ext == "K")
        size = std::stol(num) * 1024;
    else if (ext == "m" || ext == "M")
        size = std::stol(num) * 1048576;
    else if (ext == "g" || ext == "G")
        size = std::stol(num) * 1048576 * 1024;
    else
        size = std::stol(num);
    return size;
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
    bool    insideLocation = false;
    ServerConfig current;
    LocationConfig currentLoc;
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
        else if (line == "}" && insideLocation == false)
        {
            insideBlock = false;
            if (current.uploadPath.empty())
            {
                current.uploadPath = current.root + "/uploads";
            }
            servers.push_back(current);
        }
        else if (line == "}" && insideLocation == true)
        {
            insideLocation = false;
            current.locations.push_back(currentLoc);
        }
        else if (insideBlock == true)
        {
            size_t spacePos = line.find(' ');
            if (spacePos == std::string::npos)
                continue;
            std::string directive = line.substr(0, spacePos);
            if (directive == "listen" || directive == "root" || directive == "return" || directive == "location"
                    || directive == "index" || directive == "autoindex" || directive == "server_name"
                    || directive == "error_page" || directive == "upload_location"
                    || directive == "client_max_body_size" || directive == "methods" || directive == "cgi_ext")
            {
                std::string value = trim(line.substr(line.find(' ') + 1));
                if (!value.empty() && value.back() == ';')
                    value.pop_back();
                if (directive == "listen")
                    current.ports.push_back(std::stoi(value));
                else if (directive == "root")
                {
                    if (insideLocation == false)
                        current.root = value;
                    else
                        currentLoc.root = value;
                }
                else if (directive == "server_name")
                    current.serverName = value;
                else if (directive == "index")
                {
                    if (insideLocation == false)
                        current.index = value;
                    else
                        currentLoc.index = value;
                }
                else if (directive == "client_max_body_size")
                    current.maxClientBody = extractSize(value);
                else if (directive == "methods")
                    currentLoc.methods = value;
                else if (directive == "return")
                {
                    int i = 0;
                    while (std::isdigit(value[i]))
                        i++;
                    if (i > 0)
                    {
                        std::string code = value.substr(0, i);
                        currentLoc.redirCode = std::atol(code.c_str());
                    }
                    currentLoc.redirPath = trim(value.substr(i));
                }
                else if (directive == "cgi_ext")
                    currentLoc.cgiExt = value;
                else if (directive == "upload_location")
                {
                    if (insideLocation == false)
                        current.uploadPath = current.root + value;
                    else
                        currentLoc.uploadLoc = value;
                }
                else if (directive == "autoindex")
                {
                    if (value == "on")
                    {
                        if (insideLocation == false)
                            current.autoindex = 1;
                        else
                            currentLoc.autoindex = 1;
                    }
                    else
                    {
                        if (insideLocation == false)
                            current.autoindex = 0;
                        else
                            currentLoc.autoindex = 0;
                    }
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
                else if (directive == "location")
                {
                    insideLocation = true;
                    currentLoc = LocationConfig();
                    value = trim(value);
                    if (value[value.length() - 1] != '{' || !(value[0] == '/' || value[0] == '.'))
                    {
                        std::cout << value[value.length() - 1] << "-----------" << value[0] << std::endl;
                        perror("MALFORMED CONFIG FILE");
                        exit(EXIT_FAILURE);
                    }
                    currentLoc.path = trim(value.substr(0 ,value.find('{')));
                }
            }
            else
                std::cerr << "Warning: unknown directive: " << directive << std::endl;
        }
    }
}

const std::vector<ServerConfig> &ConfigParser::getServers() const
{
    return servers;
}