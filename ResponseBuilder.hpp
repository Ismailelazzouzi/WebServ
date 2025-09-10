#if !defined(RESPONSEBUILDER_HPP)
#define RESPONSEBUILDER_HPP

#include "RequestParser.hpp"


struct ClientInfo;

struct cgiData
{
    char    **env;
};

class ResponseBuilder
{
private:
    RequestParser *rp;
    LocationConfig *location;
    std::string toSend;
    int fileLen;
    cgiData *cgi;
public:
    ResponseBuilder();
    ResponseBuilder(RequestParser *rp, ServerConfig &config, ClientInfo &client, LocationConfig *location);
    const std::string getToSend() const;
    std::string getFullPath(RequestParser *rp);
    int runCgi(RequestParser *rp, ServerConfig &config, ClientInfo &client, LocationConfig *location);
    const cgiData *getCgi() const;
    std::string getRoot() const;
    std::string getIndex() const;
    std::string getPath() const;
    bool getAutoIndex() const;
};

#endif // RESPONSEBUILDER_HPP
