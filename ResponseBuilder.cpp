#include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder()
{}

ResponseBuilder::ResponseBuilder(RequestParser rp, ServerConfig &config) : rp(rp)
{
    if (rp.getMethod() == "GET" || rp.getMethod() == "DELETE" || rp.getMethod() == "POST")
    {
        std::string localPath;
        std::string path = rp.getPath();
        if (path[0] == '/')
            localPath = rp.getRoot() + path;
        else
            localPath = rp.getRoot() + "/" + path;
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
