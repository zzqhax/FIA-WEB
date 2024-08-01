//con.cpp
#include "con.h"

CON::CON() {
    listenfd = -1;
    epollfd = -1;
    std::memset(&server_addr, 0, sizeof(server_addr));
}

CON::~CON() {
    if (listenfd != -1) close(listenfd);
    if (epollfd != -1) close(epollfd);
}

void CON::create() {
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket creation failed");
        return;
    }
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll creation failed");
        return;
    }
    addFdToEpoll(listenfd, EPOLLIN);
}

void CON::bind(const char* ip, int port) {
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (::bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        return;
    }
}

void CON::listen(int backlog) {
    if (::listen(listenfd, backlog) == -1) {
        perror("listen failed");
        return;
    }
}

int CON::accept() {
    int clientfd = ::accept(listenfd, nullptr, nullptr);
    if (clientfd == -1) {
        perror("accept failed");
    }
    return clientfd;
}

void CON::addFdToEpoll(int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl ADD failed");
    }
}

void CON::modifyFdInEpoll(int fd, int events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        perror("epoll_ctl MOD failed");
    }
}

void CON::deleteFdFromEpoll(int fd) {
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        perror("epoll_ctl DEL failed");
    }
}

void CON::run(ClientHandler clientHandler) {
    struct epoll_event events[10];

    while (1) {
        int nfds = epoll_wait(epollfd, events, 10, -1);
        if (nfds == -1) {
            perror("epoll_wait failed");
            return;
        }
        for (int n = 0; n < nfds; n++) {
            if (events[n].data.fd == listenfd) {
                int clientfd = accept();
                if (clientfd != -1) {
                    addFdToEpoll(clientfd, EPOLLIN);
                }
            } else {
                clientHandler(events[n].data.fd);
            }
        }
    }
}
