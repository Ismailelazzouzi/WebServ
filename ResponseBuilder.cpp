#include "ResponseBuilder.hpp"
#include "ClientSession.hpp"

ResponseBuilder::ResponseBuilder()
{}

ResponseBuilder::ResponseBuilder(RequestParser *rp, ServerConfig &config, ClientInfo &client, LocationConfig *location) : rp(rp), location(location)
{
    if (rp->getMethod() == "GET" || rp->getMethod() == "DELETE" || rp->getMethod() == "POST")
    {
        std::string localPath;
        localPath = getRoot() + getPath();
        if (getRoot()[getRoot().length() - 1] != '/' && getPath()[0] != '/')
            localPath = getRoot() + "/" + getPath();
        std::string path = rp->getPath();
        int signal = 1;
        DIR *dir = opendir(localPath.c_str());
        if (dir && localPath[localPath.length() -1] != '/')
        {
            toSend = rp->getVersion() + " 301 Moved Permenantly\r\n" + "Location: " + rp->getFullPath()  + "/" + "\r\nContent-Length: 0" + "\r\n\r\n";
            return ;
        }
        if (location != nullptr)
        {
            if (location->redirCode != 0)
                signal = 301;
            else if (location->methods.find(rp->getMethod()) == std::string::npos)
                signal = 405;
        }
        rp->setType(localPath);
        if (signal == 1 && rp->getContentType() == "cgi" && rp->getMethod() == "GET")
        {
            signal = runCgi(rp, config, client, location);
        }
        else if (signal == 0)
            return ;
        else if (signal != 0 && signal != 1)
        {
            if (signal == 500)
            {
                std::map<int, std::string> errors = rp->getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/500.html";
                if (errors.find(500) != errors.end())
                    errorPath = rp->getRoot() + rp->getErrorPages().at(500);
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
                toSend = rp->getVersion() + " 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                return ;
            }
            else if (signal == 404)
            {
                std::map<int, std::string> errors = rp->getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/404.html";
                if (errors.find(404) != errors.end())
                    errorPath = rp->getRoot() + rp->getErrorPages().at(404);
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
                toSend = rp->getVersion() + " 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                return ;
            }
            else if (signal == 403)
            {
                std::map<int, std::string> errors = rp->getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/403.html";
                if (errors.find(403) != errors.end())
                    errorPath = rp->getRoot() + rp->getErrorPages().at(403);
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
                toSend = rp->getVersion() + " 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                return ;
            }
            else if (signal == 405)
            {
                std::map<int, std::string> errors = rp->getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/405.html";
                if (errors.find(405) != errors.end())
                    errorPath = rp->getRoot() + rp->getErrorPages().at(405);
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
                toSend = rp->getVersion() + " 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                return ;
            }
            else if (location != nullptr && signal == 301)
            {
                toSend = rp->getVersion() + " 301 Moved Permenantly\r\n" + "Location: " + location->redirPath + "\r\nContent-Length: 0" + "\r\n\r\n";
                return ;
            }
            else
                rp->setContentType("text/plain");
        }
        if (rp->getMethod() == "DELETE")
        {
            if (std::remove(localPath.c_str()) == 0)
            {
                toSend = rp->getVersion() + " 204 No Content\r\n\r\n";
                return ;
            }
            else
            {
                std::map<int, std::string> errors = rp->getErrorPages();
                std::string errorPath;
                if (errno == ENOENT)
                {
                    errorPath = "./defaultErrors/404.html";
                    if (errors.find(404) != errors.end())
                        errorPath = rp->getRoot() + rp->getErrorPages().at(404);
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
                    toSend = rp->getVersion() + " 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                    return ;
                }
                else
                {
                    errorPath = "./defaultErrors/403.html";
                    if (errors.find(403) != errors.end())
                        errorPath = rp->getRoot() + rp->getErrorPages().at(403);
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
                    toSend = rp->getVersion() + " 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                    return ;
                }
            }
        }
        if (rp->getMethod() == "POST")
        {
            if (rp->getBody().size() > rp->getMaxClientBody())
            {
                std::map<int, std::string> errors = rp->getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/413.html";
                if (errors.find(413) != errors.end())
                    errorPath = rp->getRoot() + rp->getErrorPages().at(413);
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
                toSend = rp->getVersion() + " 413 Payload Too Large\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                return ;
            }
            DIR *dir = opendir(rp->getUploadPath().c_str());
            if (!dir)
            {
                std::map<int, std::string> errors = rp->getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/500.html";
                if (errors.find(500) != errors.end())
                    errorPath = rp->getRoot() + rp->getErrorPages().at(500);
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
                toSend = rp->getVersion() + " 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                closedir(dir);
                return ;
            }
            else
            {
                std::string fullpath = getFullPath(rp);
                if (fullpath == "")
                    fullpath = rp->getUploadPath() + rp->getPath();
                std::ifstream file;
                file.open(fullpath.c_str());
                if (file.is_open())
                {
                    std::map<int, std::string> errors = rp->getErrorPages();
                    std::string errorPath;
                    errorPath = "./defaultErrors/409.html";
                    if (errors.find(409) != errors.end())
                        errorPath = rp->getRoot() + rp->getErrorPages().at(409);
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
                    toSend = rp->getVersion() + " 409 Conflict\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                    closedir(dir);
                    return ;
                }
                else
                {
                    std::ofstream outputFile(fullpath.c_str(), std::ios::binary);
                    if (!outputFile.is_open())
                    {
                        std::map<int, std::string> errors = rp->getErrorPages();
                        std::string errorPath;
                        errorPath = "./defaultErrors/500.html";
                        if (errors.find(500) != errors.end())
                            errorPath = rp->getRoot() + rp->getErrorPages().at(500);
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
                        toSend = rp->getVersion() + " 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                        closedir(dir);
                        return ;
                    }
                    else
                    {
                        outputFile.write(rp->getBody().c_str(), rp->getBody().length());
                        outputFile.close();
                        toSend = rp->getVersion() + " 201 Created\r\nLocation: ";
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
            if (getAutoIndex())
            {
                struct dirent *entry;
                std::string html = "<html><body>";
                html += "<h1>Index of " + rp->getPath() + "</h1><ul>";
                while ((entry = readdir(dir)) != NULL)
                {
                    std::string name = entry->d_name;
                    if (name == "." || name == "..")
                    continue ;
                    html += "<li><a href=\"";
                    html += rp->getPath();
                    html += "/";
                    html += name;
                    html += "\">";
                    html += name;
                    html += "</a></li>";
                }
                html += "</ul></body></html>";
                fileLen = html.length();
                toSend = rp->getVersion() + " 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
                + std::to_string(fileLen) + "\r\n\r\n" + html;
                closedir(dir);
                return ;
            }
            else if (!getAutoIndex())
            {
                std::map<int, std::string> errors = rp->getErrorPages();
                std::string errorPath;
                errorPath = "./defaultErrors/403.html";
                if (errors.find(403) != errors.end())
                errorPath = rp->getRoot() + rp->getErrorPages().at(403);
                std::ifstream errorFile(errorPath);
                if (errorFile.is_open())
                {
                    std::string errorContent((std::istreambuf_iterator<char>(errorFile)),
                    (std::istreambuf_iterator<char>()));
                    fileLen = errorContent.length();
                    toSend = rp->getVersion() + " 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
                    closedir(dir);
                    return ;
                }
            }
        }
        std::ifstream file;
        file.open(localPath);
        if (!file.is_open())
        {
            std::map<int, std::string> errors = rp->getErrorPages();
            std::string errorPath;
            errorPath = "./defaultErrors/404.html";
            if (errors.find(404) != errors.end())
                errorPath = rp->getRoot() + rp->getErrorPages().at(404);
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
            toSend = rp->getVersion() + " 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
            return ;
        }
        std::string fileContent((std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>()));
        fileLen = fileContent.length();
        std::string fileLength = std::to_string(fileLen);
        rp->setType(localPath);
        std::cout << rp->getContentType() << std::endl;
        toSend = rp->getVersion() + " 200 OK\r\nContent-Type: " + rp->getContentType() + "\r\nContent-Length: " + fileLength + "\r\n\r\n" + fileContent;
    }
    else
    {
        std::map<int, std::string> errors = rp->getErrorPages();
        std::string errorPath;
        errorPath = "./defaultErrors/405.html";
        if (errors.find(405) != errors.end())
            errorPath = rp->getRoot() + rp->getErrorPages().at(405);
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
        toSend = rp->getVersion() + " 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
        return ;
    }
}

char    **prepareEnv(RequestParser *rp, ServerConfig &config, ClientInfo &client, LocationConfig *location)
{
    std::vector<std::string> keyValue;
    std::string combined;
    combined = "REQUEST_METHOD=" + rp->getMethod();
    keyValue.push_back(combined);
    combined = "SCRIPT_NAME=" + rp->getPath();
    keyValue.push_back(combined);
    combined = "PATH_INFO=" + rp->getExtraInfo();
    keyValue.push_back(combined);    
    size_t pos = rp->getPath().find("?");
    if (pos == std::string::npos)
        combined = "QUERY_STRING=";
    else
        combined = "QUERY_STRING=" + rp->getPath().substr(pos + 1);
    std::cout << combined << std::endl;
    keyValue.push_back(combined);
    combined = "CONTENT_LENGTH=" + std::to_string(rp->getBody().length());
    keyValue.push_back(combined);
    combined = "SERVER_PORT=" + client.localPort;
    keyValue.push_back(combined);
    combined = "SERVER_PROTOCOL=" + rp->getVersion();
    keyValue.push_back(combined);
    combined = "REMOTE_ADDR=" + client.remoteIp;
    keyValue.push_back(combined);
    combined = "REMOTE_PORT=" + client.remotePort;
    keyValue.push_back(combined);
    if (location && !location->root.empty())
        combined = "DOCUMENT_ROOT=" + location->root;
    else
        combined = "DOCUMENT_ROOT=" + config.root;
    keyValue.push_back(combined);
    if (location && !location->root.empty())
        combined = "PATH_TRANSLATED=" + location->root + rp->getExtraInfo();
    else
        combined = "PATH_TRANSLATED=" + config.root + rp->getExtraInfo();
    keyValue.push_back(combined);
    combined = "REQUEST_URI=" + rp->getFullPath();
    keyValue.push_back(combined);
    combined = "GATEWAY_INTERFACE=CGI/1.1";
    keyValue.push_back(combined);
    combined = "SERVER_SOFTWARE=serverMli7";
    keyValue.push_back(combined);
    if (!rp->getConfig()->serverName.empty())
        combined = "SERVER_NAME=" + rp->getConfig()->serverName;
    else
        combined = "SERVER_NAME=" + rp->getServerName();
    keyValue.push_back(combined);
    combined = "CONTENT_TYPE=" + rp->getCt();
    keyValue.push_back(combined);
    pos = rp->getHeaders().find("\r\n");
    if (pos == std::string::npos)
        return NULL;
    std::string headers = rp->getHeaders().substr(pos + 2);
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
    char **env = new char*[keyValue.size() + 1];
    for (size_t i = 0; i < keyValue.size(); i++)
    {
        env[i] = new char[keyValue[i].length() + 1];
        int j = 0;
        while (j < keyValue[i].length())
        {
            env[i][j] = keyValue[i][j];
            j++;
        }
        env[i][j] = '\0';
    }
    env[keyValue.size()] = NULL;
    return (env);
}

void    cleanup(char **env, int *readFds, int *writeFds)
{
    if (readFds)
    {
        close(readFds[0]);
        close(readFds[1]);
    }
    if (writeFds)
    {
        close (writeFds[0]);
        close(writeFds[1]);
    }
    if (env)
    {
        size_t len = 0;
        while (env[len])
            len++;
        for (size_t i = 0; i < len; i++)
        {
            delete[] env[i];
        }
        delete[] env;
    }
}

int    ResponseBuilder::runCgi(RequestParser *rp, ServerConfig &config, ClientInfo &client, LocationConfig *location)
{
    std::string localPath;
    std::string path = rp->getPath();
    if (path[0] == '/')
        localPath = rp->getRoot() + path;
    else
        localPath = rp->getRoot() + "/" + path;
    struct stat sfile;
    if (stat(localPath.c_str(), &sfile) != 0)
        return (404);
    if (S_ISREG(sfile.st_mode) == false || (sfile.st_mode & S_IXUSR) == 0)
        return (403);
    int readfds[2];
    int writefds[2];
    if (pipe(readfds) == -1 || pipe(writefds) == -1)
        return (500);
    char **env = prepareEnv(rp, config, client, location);
    pid_t pid = fork();
    if (pid == 0)
    {
        close(writefds[1]);
        close(readfds[0]);
        dup2(writefds[0], STDIN_FILENO);
        dup2(readfds[1], STDOUT_FILENO);
        close(writefds[0]);
        close(readfds[1]);
        char *str = new char[localPath.length() + 1];
        std::strcpy(str, localPath.c_str());
        char* const argv[] = {str, NULL};
        if (execve(localPath.c_str(), argv, env) == -1)
        {
            perror("EXECVE FAILED !");
            delete[] str;
            exit(EXIT_FAILURE);
        }
    }
    if (pid > 0)
    {
        close(writefds[0]);
        close(readfds[1]);
        if (rp->getBody().length() > 0)
        {
            int bytes = 0;
            while (bytes < rp->getBody().length())
            {
                bytes += write(writefds[1], rp->getBody().c_str() + bytes, rp->getBody().length() - bytes);
            }
        }
        close(writefds[1]);
        std::string cgiOutput;
        char buffer[5000];
        size_t bytesRead;
        while ((bytesRead = read(readfds[0], buffer, sizeof(buffer))) > 0)
        {
            cgiOutput.append(buffer, bytesRead);
        }
        if (bytesRead == -1)
        {
            perror("ERROR reading from cgi pipeline");
            cleanup(env, NULL, NULL);
            close(readfds[0]);
            return (500);
        }
        close(readfds[0]);
        int status;
        waitpid(pid, &status, 0);
        cleanup(env, NULL, NULL);
        std::string cgiHeaders;
        std::string cgiBody;
        size_t pos = cgiOutput.find("\r\n\r\n");
        if (pos == std::string::npos)
            return (500);
        cgiHeaders = cgiOutput.substr(0, pos);
        cgiBody = cgiOutput.substr(pos + 4);
        toSend = rp->getVersion() + " 200 OK\r\n" + cgiHeaders + "\r\nContent-Length: " + std::to_string(cgiBody.length()) + "\r\n\r\n" + cgiBody;
        return (0);
    }
    else
    {
        cleanup(env, readfds, writefds);;
        return (500);
    }
}

std::string ResponseBuilder::getFullPath(RequestParser *rp)
{
    size_t pos = rp->getHeaders().find("Content-Disposition");
    if (pos == std::string::npos)
        return "";
    std::string line = rp->getHeaders().substr(pos);
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
    std::string fullpath = rp->getUploadPath() + filename;
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

std::string ResponseBuilder::getRoot() const
{
    if (location && !location->root.empty())
    {
        return location->root;
    }
    return rp->getRoot();
}

bool ResponseBuilder::getAutoIndex() const
{
    if (location)
        return location->autoindex;
    return rp->getAutoIndex();
}

std::string ResponseBuilder::getIndex() const
{
    if (location && !location->index.empty())
    {
        return location->index;
    }
    return rp->getIndex();
}

std::string ResponseBuilder::getPath() const
{
    if (location != nullptr)
    {
        std::string configIndex = "/" + rp->getConfig()->index;
        if (configIndex.compare(rp->getPath().c_str()) == 0)
        {
            return "/" + location->index;
        }
        int pos = rp->getPath().find(location->path);
        if (pos == std::string::npos)
            return rp->getPath();
        std::string value = rp->getPath().substr(pos + location->path.length());
        std::cout << value << std::endl;
        if (value.empty() && !location->index.empty())
            return "/" + location->index;
        return value;
    }
    return rp->getPath();
}