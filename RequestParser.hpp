#if !defined(REQUESTPARSER_HPP)
#define REQUESTPARSER_HPP

#include "MySocket.hpp"

class RequestParser
{
private:
    std::string fullRequest;
    std::string method;
    std::string path;
    std::string version;
    std::string contentType;
public:
    RequestParser();
    RequestParser(std::string buffer);
    const std::string &getMethod() const;
    const std::string &getPath() const;
    const std::string &getVersion() const;
    const std::string &getContentType() const;
    void  setType();
};



#endif // REQUESTPARSER_HPP
