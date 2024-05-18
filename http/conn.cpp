#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "config.h"
#include "bbs.h"

int main(){
    char buffer[buffersize];
    int listenfd = 0;
    int stat = 0;
    struct sockaddr_in servaddr;
    int addrlen = sizeof(servaddr);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1){
        std::cout << "socket error" << std::endl;
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    stat = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(stat == -1){
        std::cout << "bind error" << std::endl;
        exit(1);
    }
    
    stat = listen(listenfd,3);
    if(stat == -1){
        std::cout << "Listen error" << std::endl;
        exit(1);
    }
    std::cout <<"ok" << std::endl;

    while(1){
        int servfd = accept(listenfd,(struct sockaddr *)&servaddr, (socklen_t*)&addrlen);
        if(servfd == -1){
            std::cout << "Accept error" << std::endl;
        }

        stat = read(servfd,buffer,buffersize);
        if(stat == -1){
            std::cout << "Read error" << std::endl;
        }
        std::cout << buffer << std::endl;

        close(servfd);
    }
    close(listenfd);
    return(0);
}