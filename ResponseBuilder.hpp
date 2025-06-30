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
    ResponseBuilder(RequestParser rp, std::string root);
    const std::string &getToSend() const;
    int getFileLen() const;
};


#endif // RESPONSEBUILDER_HPP
