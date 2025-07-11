#ifndef MYSOCKET_HPP
#define MYSOCKET_HPP

#include <iostream>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <fstream>
#include <poll.h> 
#include <cctype>

class MySocket
{
private:
    struct sockaddr_in adress;
    int sock;
public:
    MySocket(int domain, int service, int protocol, int port, u_long interface);
    void    testConnection(int item);
    const struct sockaddr_in &getAdress() const;
    int getSock() const;
};

#endif