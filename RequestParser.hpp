#if !defined(REQUESTPARSER_HPP)
#define REQUESTPARSER_HPP

#include "MySocket.hpp"
#include "ConfigParser.hpp"

class RequestParser
{
private:
    ServerConfig config;
    std::string fullRequest;
    std::string requestBody;
    std::string requestHeaders;
    std::string method;
    std::string path;
    std::string version;
    std::string contentType;
public:
    RequestParser();
    RequestParser(std::string &buffer, ServerConfig &config);
    const ServerConfig &getConfig() const;
    const std::string &getMethod() const;
    const std::string &getPath() const;
    const std::string &getVersion() const;
    const std::string &getContentType() const;
    const std::string &getIndex() const;
    const std::string &getUploadPath() const;
    const std::string &getBody() const;
    const std::string &getHeaders() const;
    const bool getAutoIndex() const;
    std::map<int, std::string> getErrorPages();
    const std::string &getRoot() const;
    int getMaxClientBody() const;
    void  setType();
    void setContentType(std::string type);
};



#endif // REQUESTPARSER_HPP