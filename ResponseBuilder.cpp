#include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder(RequestParser rp) : rp(rp)
{
    std::string localPath;
    std::string path = rp.getPath();
    localPath = "./serverfiles" + path;
    std::ifstream file;
    file.open(localPath);
    if (!file.is_open())
    {
        toSend = rp.getVersion() + " 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 35\r\n\r\n<html><h1>404 NOT FOUND</h1></html>";
        return ;
    }
    std::string fileContent((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));
    fileLen = fileContent.length();
    std::string fileLength = std::to_string(fileLen);
    rp.setType();
    toSend = rp.getVersion() + " 200 OK\r\nContent-Type: " + rp.getContentType() + "\r\nContent-Length: " + fileLength + "\r\n\r\n" + fileContent;
}

const std::string &ResponseBuilder::getToSend() const
{
    return toSend;
}

int ResponseBuilder::getFileLen() const
{
    return fileLen;
}