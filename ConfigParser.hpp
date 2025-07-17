#if !defined(CONFIGPARSER_HPP)
#define CONFIGPARSER_HPP

#include "MySocket.hpp"

struct ServerConfig
{
    int port;
    std::string root;
    std::string index;
    std::map<int, std::string> errorPages;
    bool autoindex;
    size_t  maxClientBody;
    std::string uploadPath;
};

class ConfigParser
{
private:
    std::vector<ServerConfig> servers;
public:
    ConfigParser();
    ~ConfigParser();
    void    parse(const std::string &filepath);
    const std::vector<ServerConfig> &getServers() const;
};


#endif // CONFIGPARSER_HPP
