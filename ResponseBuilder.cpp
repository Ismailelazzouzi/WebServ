#include "ResponseBuilder.hpp"
#include "ClientSession.hpp"

ResponseBuilder::ResponseBuilder()
{}

ResponseBuilder::ResponseBuilder(RequestParser rp, ServerConfig &config, ClientInfo &client) : rp(rp)
{
    if (rp.getMethod() == "GET" || rp.getMethod() == "DELETE" || rp.getMethod() == "POST")
    {
        std::string localPath;
        std::string path = rp.getPath();
        int signal;
        if (path[0] == '/')
            localPath = rp.getRoot() + path;
        else
            localPath = rp.getRoot() + "/" + path;
        rp.setType();
        if (rp.getContentType() == "cgi")
            signal = runCgi(rp, config, client);
        // if (signal == 0)
        //     return;
        // else
        // {
        //     if (signal == 404 || signal == 403 || signal == 500)
        //     {
        //         //sendCustomError(signal);
        //         return;
        //     }
        //     else
        //         rp.setContentType("text/plain");
        // }
        if (rp.getMethod() == "DELETE")
        {
            if (std::remove(localPath.c_str()) == 0)
            {
                toSend = rp.getVersion() + " 204 No Content\r\n\r\n";
                return ;
            }
            else
            {
                std::map<int, std::string> errors = rp.getErrorPages();
                std::string errorPath;
                if (errno == ENOENT)
                {
                    errorPath = "./defaultErrors/404.html";
                    if (errors.find(404) != errors.end())
                        errorPath = rp.getRoot() + rp.getErrorPages().at(404);
                    std::ifstream errorFile(errorPath);
                    std::string errorContent;
                    if (!errorFile.is_open())
                    {
                        errorPath = "./defaultErrors/404.html";
                        std::ifstream defaultErrorFile(errorPath);
                        errorContent = std::string((std::istreambuf_iterator<char>(defaultErrorFile)),
                        (std::istreambuf_iterator<char>()));
                    }
                    else
                    {
                        errorContent = std::string((std::istreambuf_iterator<char>(errorFile)),
                            (std::istreambuf_iterator<char>()));
                    }
                    fileLen = errorContent.length();
                    toSend = rp.getVersion() + " 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                    return ;
                }
                else
                {
                    errorPath = "./defaultErrors/403.html";
                    if (errors.find(403) != errors.end())
                        errorPath = rp.getRoot() + rp.getErrorPages().at(403);
                    std::ifstream errorFile(errorPath);
                    std::string errorContent;
                    if (!errorFile.is_open())
                    {
                        errorPath = "./defaultErrors/403.html";
                        std::ifstream defaultErrorFile(errorPath);
                        errorContent = std::string((std::istreambuf_iterator<char>(defaultErrorFile)),
                        (std::istreambuf_iterator<char>()));
                    }
                    else
                    {
                        errorContent = std::string((std::istreambuf_iterator<char>(errorFile)),
                            (std::istreambuf_iterator<char>()));
                    }
                    fileLen = errorContent.length();
                    toSend = rp.getVersion() + " 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                    return ;
                }
            }
        }
        if (rp.getMethod() == "POST")
        {
            if (rp.getBody().size() > rp.getMaxClientBody())
            {
                std::map<int, std::string> errors = rp.getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/413.html";
                if (errors.find(413) != errors.end())
                    errorPath = rp.getRoot() + rp.getErrorPages().at(413);
                std::ifstream errorFile(errorPath);
                std::string errorContent;
                if (!errorFile.is_open())
                {
                    errorPath = "./defaultErrors/413.html";
                    std::ifstream defaultErrorFile(errorPath);
                    errorContent = std::string((std::istreambuf_iterator<char>(defaultErrorFile)),
                    (std::istreambuf_iterator<char>()));
                }
                else
                {
                    errorContent = std::string((std::istreambuf_iterator<char>(errorFile)),
                        (std::istreambuf_iterator<char>()));
                }
                fileLen = errorContent.length();
                toSend = rp.getVersion() + " 413 Payload Too Large\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                return ;
            }
            DIR *dir = opendir(rp.getUploadPath().c_str());
            if (!dir)
            {
                std::map<int, std::string> errors = rp.getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/500.html";
                if (errors.find(500) != errors.end())
                    errorPath = rp.getRoot() + rp.getErrorPages().at(500);
                std::ifstream errorFile(errorPath);
                std::string errorContent;
                if (!errorFile.is_open())
                {
                    errorPath = "./defaultErrors/500.html";
                    std::ifstream defaultErrorFile(errorPath);
                    errorContent = std::string((std::istreambuf_iterator<char>(defaultErrorFile)),
                        (std::istreambuf_iterator<char>()));
                }
                else
                {
                    errorContent = std::string((std::istreambuf_iterator<char>(errorFile)),
                        (std::istreambuf_iterator<char>()));
                }
                fileLen = errorContent.length();
                toSend = rp.getVersion() + " 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                closedir(dir);
                return ;
            }
            else
            {
                std::string fullpath = getFullPath(rp);
                if (fullpath == "")
                    fullpath = rp.getUploadPath() + rp.getPath();
                std::ifstream file;
                file.open(fullpath.c_str());
                if (file.is_open())
                {
                    std::map<int, std::string> errors = rp.getErrorPages();
                    std::string errorPath;
                    errorPath = "./defaultErrors/409.html";
                    if (errors.find(409) != errors.end())
                        errorPath = rp.getRoot() + rp.getErrorPages().at(409);
                    std::ifstream errorFile(errorPath);
                    std::string errorContent;
                    if (!errorFile.is_open())
                    {
                        errorPath = "./defaultErrors/409.html";
                        std::ifstream defaultErrorFile(errorPath);
                        errorContent = std::string((std::istreambuf_iterator<char>(defaultErrorFile)),
                            (std::istreambuf_iterator<char>()));
                    }
                    else
                    {
                        errorContent = std::string((std::istreambuf_iterator<char>(errorFile)),
                            (std::istreambuf_iterator<char>()));
                    }
                    fileLen = errorContent.length();
                    toSend = rp.getVersion() + " 409 Conflict\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                    closedir(dir);
                    return ;
                }
                else
                {
                    std::ofstream outputFile(fullpath.c_str(), std::ios::binary);
                    if (!outputFile.is_open())
                    {
                        std::map<int, std::string> errors = rp.getErrorPages();
                        std::string errorPath;
                        errorPath = "./defaultErrors/500.html";
                        if (errors.find(500) != errors.end())
                            errorPath = rp.getRoot() + rp.getErrorPages().at(500);
                        std::ifstream errorFile(errorPath);
                        std::string errorContent;
                        if (!errorFile.is_open())
                        {
                            errorPath = "./defaultErrors/500.html";
                            std::ifstream defaultErrorFile(errorPath);
                            errorContent = std::string((std::istreambuf_iterator<char>(defaultErrorFile)),
                                (std::istreambuf_iterator<char>()));
                        }
                        else
                        {
                            errorContent = std::string((std::istreambuf_iterator<char>(errorFile)),
                                (std::istreambuf_iterator<char>()));
                        }
                        fileLen = errorContent.length();
                        toSend = rp.getVersion() + " 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                        closedir(dir);
                        return ;
                    }
                    else
                    {
                        outputFile.write(rp.getBody().c_str(), rp.getBody().length());
                        outputFile.close();
                        toSend = rp.getVersion() + " 201 Created\r\nLocation: ";
                        toSend += fullpath;
                        toSend += "\r\nContent-Length: 0\r\n\r\n";
                        closedir(dir);
                        return ;
                    }
                }
            }
            return ;
        }
        DIR *dir = opendir(localPath.c_str());
        if (dir)
        {
            if (rp.getAutoIndex())
            {
                struct dirent *entry;
                std::string html = "<html><body>";
                html += "<h1>Index of " + rp.getPath() + "</h1><ul>";
                while ((entry = readdir(dir)) != NULL)
                {
                    std::string name = entry->d_name;
                    if (name == "." || name == "..")
                    continue ;
                    html += "<li><a href=\"";
                    html += rp.getPath();
                    html += "/";
                    html += name;
                    html += "\">";
                    html += name;
                    html += "</a></li>";
                }
                html += "</ul></body></html>";
                fileLen = html.length();
                toSend = rp.getVersion() + " 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
                + std::to_string(fileLen) + "\r\n\r\n" + html;
                closedir(dir);
                return ;
            }
            else if (!rp.getAutoIndex())
            {
                std::map<int, std::string> errors = rp.getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/403.html";
                if (errors.find(403) != errors.end())
                errorPath = rp.getRoot() + rp.getErrorPages().at(403);
                std::ifstream errorFile(errorPath);
                if (errorFile.is_open())
                {
                    std::string errorContent((std::istreambuf_iterator<char>(errorFile)),
                    (std::istreambuf_iterator<char>()));
                    fileLen = errorContent.length();
                    toSend = rp.getVersion() + " 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                    closedir(dir);
                    return ;
                }
            }
        }
        std::ifstream file;
        file.open(localPath);
        if (!file.is_open())
        {
            std::map<int, std::string> errors = rp.getErrorPages();
            std::string errorPath;
            errorPath = "./defaultErrors/404.html";
            if (errors.find(404) != errors.end())
                errorPath = rp.getRoot() + rp.getErrorPages().at(404);
            std::ifstream errorFile(errorPath);
            std::string errorContent;
            if (!errorFile.is_open())
            {
                errorPath = "./defaultErrors/404.html";
                std::ifstream defaultErrorFile(errorPath);
                errorContent = std::string((std::istreambuf_iterator<char>(defaultErrorFile)),
                    (std::istreambuf_iterator<char>()));
            }
            else
            {
                errorContent = std::string((std::istreambuf_iterator<char>(errorFile)),
                    (std::istreambuf_iterator<char>()));
            }
            fileLen = errorContent.length();
            toSend = rp.getVersion() + " 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
            return ;
        }
        std::string fileContent((std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>()));
        fileLen = fileContent.length();
        std::string fileLength = std::to_string(fileLen);
        rp.setType();
        toSend = rp.getVersion() + " 200 OK\r\nContent-Type: " + rp.getContentType() + "\r\nContent-Length: " + fileLength + "\r\n\r\n" + fileContent;
    }
    else
    {
        std::map<int, std::string> errors = rp.getErrorPages();
        std::string errorPath;
        errorPath = "./defaultErrors/405.html";
        if (errors.find(405) != errors.end())
            errorPath = rp.getRoot() + rp.getErrorPages().at(405);
        std::ifstream errorFile(errorPath);
        std::string errorContent;
        if (!errorFile.is_open())
        {
            errorPath = "./defaultErrors/405.html";
            std::ifstream defaultErrorFile(errorPath);
            errorContent = std::string((std::istreambuf_iterator<char>(defaultErrorFile)),
            (std::istreambuf_iterator<char>()));
        }
        else
        {
            errorContent = std::string((std::istreambuf_iterator<char>(errorFile)),
                (std::istreambuf_iterator<char>()));
        }
        fileLen = errorContent.length();
        toSend = rp.getVersion() + " 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
        return ;
    }
}

void    prepareEnv(RequestParser rp, ServerConfig &config, ClientInfo &client)
{
    std::vector<std::string> keyValue;
    std::string combined;
    combined = "REQUEST_METHOD=" + rp.getMethod();
    keyValue.push_back(combined);
    combined = "SCRIPT_NAME=" + rp.getPath();
    keyValue.push_back(combined);
    combined = "PATH_INFO=" + rp.getExtraInfo();
    keyValue.push_back(combined);    
    size_t pos = rp.getPath().find("?");
    if (pos == std::string::npos)
        combined = "QUERY_STRING=";
    else
        combined = "QUERY_STRING=" + rp.getPath().substr(pos + 1);
    keyValue.push_back(combined);
    combined = "CONTENT_LENGTH=" + std::to_string(rp.getBody().length());
    keyValue.push_back(combined);
    combined = "SERVER_PORT=" + std::to_string(rp.getConfig().port);
    keyValue.push_back(combined);
    combined = "SERVER_PROTOCOL=" + rp.getVersion();
    keyValue.push_back(combined);
    combined = "REMOTE_ADDR=" + client.remoteIp;
    keyValue.push_back(combined);
    combined = "REMOTE_PORT=" + client.remotePort;
    keyValue.push_back(combined);
    combined = "DOCUMENT_ROOT=" + config.root;
    keyValue.push_back(combined);
    combined = "PATH_TRANSLATED=" + config.root + rp.getExtraInfo();
    keyValue.push_back(combined);
    combined = "REQUEST_URI=" + rp.getFullPath();
    keyValue.push_back(combined);
    combined = "GATEWAY_INTERFACE=CGI/1.1";
    keyValue.push_back(combined);
    combined = "SERVER_SOFTWARE=serverMli7";
    keyValue.push_back(combined);
    combined = "SERVER_NAME=" + rp.getServerName();
    keyValue.push_back(combined);
    combined = "CONTENT_TYPE=" + rp.getCt();
    keyValue.push_back(combined);
    pos = rp.getHeaders().find("\r\n");
    if (pos == std::string::npos)
        return ;
    std::string headers = rp.getHeaders().substr(pos + 2);
    std::string headerName;
    size_t pos2;
    size_t pos3;
    std::string headerContent;
    while (1)
    {
        pos2 = headers.find(":");
        if (pos2 == std::string::npos)
        break;
        headerName = "HTTP_" + headers.substr(0, pos2);
        for (size_t i = 0; i < headerName.length(); i++)
        {
            headerName[i] = std::toupper(headerName[i]);
            if (headerName[i] == '-')
            headerName[i] = '_';
        }
        headerContent = headers.substr(pos2 + 2);
        pos3 = headerContent.find("\r\n");
        if (pos3 == std::string::npos)
            headerContent = headerContent.substr(0);
        else
            headerContent = headerContent.substr(0, pos3);
        if (headerName == "HTTP_CONTENT_TYPE" || headerName == "HTTP_CONTENT_LENGTH" || headerName == "HTTP_HOST")
        {
        }
        else
        {
            combined = headerName + "=" + headerContent;
            keyValue.push_back(combined);
        }
        pos = headers.find("\r\n");
        if (pos == std::string::npos)
            break;
        headers = headers.substr(pos + 2);
    }
    for (size_t i = 0; i < keyValue.size(); i++)
    {
        std::cout << keyValue[i] << std::endl;
    }
    
}

int    ResponseBuilder::runCgi(RequestParser rp, ServerConfig &config, ClientInfo &client)
{
    std::string localPath;
    std::string path = rp.getPath();
    if (path[0] == '/')
        localPath = rp.getRoot() + path;
    else
        localPath = rp.getRoot() + "/" + path;
    struct stat sfile;
    if (stat(localPath.c_str(), &sfile) != 0)
        return (404);
    if (S_ISREG(sfile.st_mode) == false || (sfile.st_mode & S_IXUSR) == 0)
        return (403);
    int readfds[2];
    int writefds[2];
    if (pipe(readfds) == -1 || pipe(writefds) == -1)
        return (500);
    prepareEnv(rp, config, client);
    return (0);
}

std::string ResponseBuilder::getFullPath(RequestParser rp)
{
    size_t pos = rp.getHeaders().find("Content-Disposition");
    if (pos == std::string::npos)
        return "";
    std::string line = rp.getHeaders().substr(pos);
    pos = line.find("\n");
    if (pos == std::string::npos)
        return "";
    line = line.substr(0, pos);
    pos = line.find("filename=");
    if (pos == std::string::npos)
        return "";
    std::string filename;
    filename = line.substr(pos + 10);
    pos = filename.find("\"");
    if (pos == std::string::npos)
        return "";
    filename = filename.substr(0, pos);
    std::string fullpath = rp.getUploadPath() + filename;
    return fullpath;
}

const std::string &ResponseBuilder::getToSend() const
{
    return toSend;
}

const cgiData *ResponseBuilder::getCgi() const
{
    return cgi;
}