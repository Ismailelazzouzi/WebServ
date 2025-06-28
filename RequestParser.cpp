#include "RequestParser.hpp"

RequestParser::RequestParser()
{
}

RequestParser::RequestParser(std::string buffer) : fullRequest(buffer)
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
        path = "/index.html";
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