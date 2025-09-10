#include "RequestParser.hpp"

RequestParser::RequestParser()
{
}

RequestParser::RequestParser(std::string &buffer, ServerConfig *config) : config(config), fullRequest(buffer)
{
    std::string firstLine;
    std::string splited[3];
    int i = 0;
    while (i + 1 < buffer.length())
    {
        if (buffer[i] == '\r' && buffer[i + 1] == '\n')
            break;
        i++;
    }
    firstLine = buffer.substr(0, i);
    i = 0;
    int j = 0;
    int k = 0;
    while (i < firstLine.length() && k < 3)
    {
        if (firstLine[i] == ' ')
        {
            splited[k] = firstLine.substr(j, i - j);
            j = i + 1;
            k++;
        }
        i++;
    }
    splited[k] = firstLine.substr(j);
    method = splited[0];
    if (splited[1] == "/")
        fullpath = "/" + config->index;
    else
        fullpath = splited[1];
    version = splited[2];
    size_t pos = fullRequest.find("\r\n\r\n");
    requestHeaders = fullRequest.substr(0, pos);
    requestBody = fullRequest.substr(pos + 4);
    if (method == "GET")
        requestBody.clear();
    pos = requestHeaders.find("Host: ");
    if (pos == std::string::npos)
        pos = requestHeaders.find("host: ");
    serverName = requestHeaders.substr(pos + 6);
    pos = serverName.find(":");
    serverName = serverName.substr(0, pos);
    pos = requestHeaders.find("Content-Type: ");
    if (pos == std::string::npos)
        pos = requestHeaders.find("content-type: ");
    if (pos != std::string::npos)
    {
        ct = requestHeaders.substr(pos + 14);
        pos = ct.find("\r\n");
        ct = ct.substr(0, pos);
    }
    pos = fullpath.find(".");
    if (pos != std::string::npos)
        extraInfo = fullpath.substr(pos);
    pos = extraInfo.find("/");
    if (pos == std::string::npos)
    {
        extraInfo.clear();
        path = fullpath.substr(0);
        return ;
    }
    else
        extraInfo = extraInfo.substr(pos);
    pos = fullpath.find(extraInfo);
    if (pos != std::string::npos)
        path = fullpath.substr(0, pos);
}



const ServerConfig *RequestParser::getConfig() const
{
    return config;
}

LocationConfig *RequestParser::getLocation()
{
    return location;
}

const std::string &RequestParser::getMethod() const
{
    return method;
}

const std::string &RequestParser::getPath() const
{
    return path;
}

const std::string &RequestParser::getFullPath() const
{
    return fullpath;
}

const std::string &RequestParser::getExtraInfo() const
{
    return extraInfo;
}

const std::string &RequestParser::getServerName() const
{
    return serverName;
}

const std::string &RequestParser::getCt() const
{
    return ct;
}

const std::string &RequestParser::getVersion() const
{
    return version;
}

const std::string &RequestParser::getContentType() const
{
    return contentType;
}

const std::string &RequestParser::getIndex() const
{
    return config->index;
}

const std::string &RequestParser::getBody() const
{
    return requestBody;
}

const std::string &RequestParser::getHeaders() const
{
    return requestHeaders;
}

const bool RequestParser::getAutoIndex() const
{
    return config->autoindex;
}

std::map<int, std::string> RequestParser::getErrorPages()
{
    return config->errorPages;
}

const std::string &RequestParser::getRoot() const
{
    return config->root;
}

const std::string &RequestParser::getUploadPath() const
{
    return config->uploadPath;
}

int RequestParser::getMaxClientBody() const
{
    return config->maxClientBody;
}

void    RequestParser::setType(std::string &path)
{
    int i = path.length() - 1;
    std::cout << path << std::endl;
    while (i > 0)
    {
        if (path[i] == '.')
            break;
        i--;
    }
    if (i == path.length() || path.length() - i < 2)
    {
        contentType = "text/plain";
        return ;
    }
    i++;
    std::string type = path.substr(i, path.length() - i);
    int j = i;
    i = 0;
    while (i < type.length())
    {
        if (type[i] >= 'A' && type[i] <= 'Z')
            type[i] = std::tolower(type[i]);
        i++;
    }
    if (type == "html")
        contentType = "text/html";
    else if (type == "js")
        contentType = "application/javascript";
    else if (type == "css")
        contentType = "text/css";
    else if (type == "json")
        contentType = "application/json";
    else if (type == "png")
        contentType = "image/png";
    else if (type == "jpg")
        contentType = "image/jpeg";
    else if (type == "php" || type == "cgi" || type == "py")
        contentType = "cgi";
    else
        contentType = "text/plain";
}

void    RequestParser::setContentType(std::string type)
{
    contentType = type;
}