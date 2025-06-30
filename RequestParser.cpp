#include "RequestParser.hpp"

RequestParser::RequestParser()
{
}

RequestParser::RequestParser(std::string buffer, std::string defaultPath) : fullRequest(buffer)
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
        path = defaultPath;
    else
        path = splited[1];
    version = splited[2];
}

const std::string &RequestParser::getMethod() const
{
    return method;
}

const std::string &RequestParser::getPath() const
{
    return path;
}

const std::string &RequestParser::getVersion() const
{
    return version;
}

const std::string &RequestParser::getContentType() const
{
    return contentType;
}

void    RequestParser::setType()
{
    int i = 0;
    while (i < path.length())
    {
        if (path[i] == '.')
            break;
        i++;
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
    else
        contentType = "text/plain";
}