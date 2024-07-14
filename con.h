#include "pch.h"

class CON{
    public:
    using ClientHandler = std::function<void(int)>;

    CON(){
        listenfd = -1;
        epollfd =  -1;
        std::memset(&server_addr, 0, sizeof(server_addr));

    };
    ~CON(){
        close(listenfd);
        close(epollfd);
    };

    void create(){
        listenfd = socket(AF_INET,SOCK_STREAM,0);
        epollfd = epoll_create1(0);
        addFdToEpoll(listenfd,EPOLLIN);
    }

    void bind(const char* ip,int port){
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip);
        server_addr.sin_port = htons(port);

        ::bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }

    void listen(int backlog){
        ::listen(listenfd,backlog);
    }

    int accept(){
        return ::accept(listenfd,nullptr,nullptr);
    }
    
    void addFdToEpoll(int fd,int events){
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
    }

    void modifyFdInEpoll(int fd,int events){
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
    }

    void deleteFdFromEpoll(int fd){
        epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,nullptr);
    }

    void run(ClientHandler clientHandler){
        struct epoll_event events[10];

        std::cout << "cccc" << std::endl;

        while(1){
            int nfds = epoll_wait(epollfd,events,10,-1);
            for(int n = 0;n < nfds;n++){
                if(events[n].data.fd == listenfd){
                    int clientfd = accept();
                    addFdToEpoll(clientfd,EPOLLIN);
                }
                else{
                    clientHandler(events[n].data.fd);
                }
            }
        }
    }

    private:
    int listenfd;
    int epollfd;
    struct sockaddr_in server_addr;
};

