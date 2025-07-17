#if !defined(RESPONSEBUILDER_HPP)
#define RESPONSEBUILDER_HPP

#include "RequestParser.hpp"

class ResponseBuilder
{
private:
    RequestParser rp;
    std::string toSend;
    int fileLen;
public:
    ResponseBuilder();
    ResponseBuilder(RequestParser rp, ServerConfig &config);
    const std::string &getToSend() const;
    std::string getFullPath(RequestParser rp);
};


#endif // RESPONSEBUILDER_HPP
