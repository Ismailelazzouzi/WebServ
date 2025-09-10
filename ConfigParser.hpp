#if !defined(CONFIGPARSER_HPP)
#define CONFIGPARSER_HPP

#include "MySocket.hpp"

struct LocationConfig
{
    std::string methods;
    std::string path;
    std::string root;
    std::string index;
    std::string redirPath;
    std::string cgiExt;
    std::string uploadLoc;
    int         redirCode;
    bool autoindex;
};

struct ServerConfig
{
    std::vector<int> ports;
    std::vector<LocationConfig>locations;
    std::string serverName;
    std::string root;
    std::string index;
    std::string clientIp;
    std::string clientPort;
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


#endif
