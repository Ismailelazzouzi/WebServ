#include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder(RequestParser rp, std::string root) : rp(rp)
{
    std::string localPath;
    std::string path = rp.getPath();
    if (path[0] == '/')
        localPath = root + path;
    else
        localPath = root + "/" + path;
    std::ifstream file;
    file.open(localPath);
    DIR *dir = opendir(localPath.c_str());
    if (dir)
    {
        std::string indexpath;
        if (localPath[localPath.length() - 1] == '/')
            indexpath = localPath + rp.getIndex();
        else
            indexpath = localPath + "/" + rp.getIndex();
        std::ifstream indexFile(indexpath);
        if (indexFile.is_open())
        {
            std::string fileContent((std::istreambuf_iterator<char>(indexFile)),
                (std::istreambuf_iterator<char>()));
            fileLen = fileContent.length();
            std::string fileLength = std::to_string(fileLen);
            rp.setType();
            toSend = rp.getVersion() + " 200 OK\r\nContent-Type: " + rp.getContentType() + "\r\nContent-Length: " + fileLength + "\r\n\r\n" + fileContent;
            closedir(dir);
            return ;
        }
        else if (rp.getAutoIndex())
        {
            struct dirent *entry;
            std::string html = "<html><body>";
            html += "<h1>Index of " + rp.getPath() + "</h1><ul>";
            while ((entry = readdir(dir)) != NULL)
            {
                std::string name = entry->d_name;
                if (name == "." || name == "..")
                    continue ;
                html += "<li><a href=\"" ;
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
            toSend = rp.getVersion() + " 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: 35\r\n\r\n<html><h1>403 FORBIDDEN</h1></html>";
            closedir(dir);
            return ;
        }
        closedir(dir);
    }
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