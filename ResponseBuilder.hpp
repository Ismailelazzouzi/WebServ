#if !defined(RESPONSEBUILDER_HPP)
#define RESPONSEBUILDER_HPP

#include "RequestParser.hpp"
#include "ClientSession.hpp"

struct cgiData
{
    char    **env;
};

class ResponseBuilder
{
private:
    RequestParser rp;

    std::string toSend;
    int fileLen;
    cgiData *cgi;
public:
    ResponseBuilder();
    ResponseBuilder(RequestParser rp, ServerConfig &config, ClientInfo &client);
    const std::string &getToSend() const;
    std::string getFullPath(RequestParser rp);
    int runCgi(RequestParser rp, ServerConfig &config, ClientInfo &client);
    const cgiData *getCgi() const;
};


#endif // RESPONSEBUILDER_HPP
