#ifndef MYSOCKET_HPP
#define MYSOCKET_HPP

#include <iostream>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <fstream>
#include <poll.h> 
#include <cctype>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>


class MySocket
{
private:
    struct sockaddr_in adress;
    int sock;
public:
    MySocket();
    MySocket(int domain, int service, int protocol, int port, std::string interface);
    void    testConnection(int item);
    const struct sockaddr_in &getAdress() const;
    int getSock() const;
};

#endif