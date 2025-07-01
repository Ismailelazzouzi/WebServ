#if !defined(REQUESTPARSER_HPP)
#define REQUESTPARSER_HPP

#include "MySocket.hpp"
#include "ConfigParser.hpp"

class RequestParser
{
private:
    std::string fullRequest;
    std::string method;
    std::string path;
    std::string version;
    std::string contentType;
    std::string index;
    bool autoindex;
public:
    RequestParser();
    RequestParser(std::string buffer, ServerConfig config);
    const std::string &getMethod() const;
    const std::string &getPath() const;
    const std::string &getVersion() const;
    const std::string &getContentType() const;
    const std::string &getIndex() const;
    const bool getAutoIndex() const;
    void  setType();
};



#endif // REQUESTPARSER_HPP
