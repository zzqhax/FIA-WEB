//
// Created by Fiametta on 2023/11/6.
//

#ifndef FIA_WEB_FIA_SERV_FIACLIT_H
#define FIA_WEB_FIA_SERV_FIACLIT_H


class FiaClit {

};


#endif //FIA_WEB_FIA_SERV_FIACLIT_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

using namespace std;
#define PORT 8080
void UDP(){
    int sockfd;
    char buffer[1024];
    char mesg[1024] = "$$";

    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n, len;//输入用户名
    cout << "please input your user name:";
    cin >> mesg + 2;
    sendto(sockfd, (const char *)mesg, strlen(mesg), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&servaddr, (socklen_t *)&len);
    buffer[n] = '\0';
    cout << "recive your name :" << buffer+2 << endl;

    while(1){
        cout << "input your message,usage:username@message" << endl;
        cin >> mesg;
        sendto(sockfd, (const char *)mesg, strlen(mesg), MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        cout << mesg <<" sent.\n";
        n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&servaddr, (socklen_t *)&len);
        buffer[n] = '\0';
        cout << "Server : " << buffer << endl;
    }
    close(sockfd);
    return;
}