#if !defined(REQUESTPARSER_HPP)
#define REQUESTPARSER_HPP

#include "ConfigParser.hpp"

class RequestParser
{
private:
    ServerConfig *config;
    std::string fullRequest;
    std::string requestBody;
    std::string requestHeaders;
    std::string method;
    std::string path;
    std::string fullpath;
    std::string extraInfo;
    std::string version;
    std::string contentType;
    std::string serverName;
    std::string ct;
    std::string uploadName;
    LocationConfig *location;
    size_t         requestLength;
public:
    RequestParser();
    RequestParser(std::string &buffer, ServerConfig *config);
    const ServerConfig *getConfig() const;
    const std::string &getMethod() const;
    const std::string &getPath() const;
    const std::string &getFullPath() const;
    const std::string &getExtraInfo() const;
    const std::string &getVersion() const;
    const std::string &getContentType() const;
    const std::string &getServerName() const;
    const std::string &getCt() const;
    const std::string &getIndex() const;
    const std::string &getUploadPath() const;
    const std::string &getBody() const;
    const std::string &getHeaders() const;
    const std::string &getFullRequest() const;
    const std::string &getUploadName() const;
    const bool getAutoIndex() const;
    std::map<int, std::string> getErrorPages();
    const std::string &getRoot() const;
    int getMaxClientBody() const;
    void  setType(std::string &path);
    void setContentType(std::string type);
    LocationConfig *getLocation();

};

    

#endif // REQUESTPARSER_HPP