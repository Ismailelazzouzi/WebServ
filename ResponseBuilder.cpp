#include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder(RequestParser rp, std::string root) : rp(rp)
{
    std::string localPath;
    std::string path = rp.getPath();
    if (path[0] == '/')
        localPath = root + path;
    else
        localPath = root + "/" + path;
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
        toSend = rp.getVersion() + " 404 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(fileLen) + "\r\n\r\n" + errorContent;
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