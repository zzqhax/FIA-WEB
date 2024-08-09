//con.h
#ifndef CON_H
#define CON_H

#include "pch.h"

class CON {
public:
    using ClientHandler = std::function<void(int)>;

    CON();
    ~CON();

    void create();
    void bind(const char* ip, int port);
    void listen(int backlog);
    int accept();
    
    void addFdToEpoll(int fd, int events);
    void modifyFdInEpoll(int fd, int events);
    void deleteFdFromEpoll(int fd);

    void run(ClientHandler clientHandler);

private:
    int listenfd;
    int epollfd;
    struct sockaddr_in server_addr;
};

#endif // CON_H
