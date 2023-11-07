//
// Created by Fiametta on 2023/11/6.
//

#ifndef FIA_WEB_FIASERV_H
#define FIA_WEB_FIASERV_H


class FiaServ {

};


#endif //FIA_WEB_FIASERV_H
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <string.h>
#include <algorithm>

using namespace std;
#define PORT 8080

string deleteSpace(char s[]){
    string res = "";
    for(int i = 0;s[i]!='\0';i++){
        if(s[i] != ' '){
            res += s[i];
        }
    }
    return res;
}

void UDP(){
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr, cliaddr;
    unordered_map<string,sockaddr_in>IPToName;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n;
    len = sizeof(cliaddr); //len is value/resuslt
    while(1){
        n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&cliaddr, (socklen_t *)&len);
        buffer[n] = '\0';
        cout << "Client : " << buffer << endl;
        cout << buffer << " sent.\n";
        cout << "----------------------------------------"<<endl;
        for(auto it = IPToName.begin();it !=IPToName.end();it++){
            cout << it->first << ntohs(it->second.sin_port) << endl;
        }
        cout << "----------------------------------------"<<endl;
        if(buffer[0] == '$' && buffer[1] == '$'){
            string username = deleteSpace(buffer+2);
            cout <<"name save : " <<  username << "IP : ";
            IPToName[username] = cliaddr;
            cout <<  ntohs(cliaddr.sin_port) << endl;//在这里我想打印出发送客户端的地址应该怎么做
            sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
            cout << buffer << " sent.\n";
        }
        else{

            int pos = 0;
            char target[1024];
            char mesg[1024];
            char *token;
            token = strtok(buffer,"@");
            strcpy(target,token);
            cout << token << endl;
            token = strtok(NULL,"@");
            strcpy(mesg,token);
            cout << mesg << endl;
            string temp = deleteSpace(target);
            cout << "temp = " << temp << endl;
            cliaddr = IPToName[temp];



            auto it = IPToName.find(temp);
            if(it == IPToName.end()){
                printf("cant find\n");
            } else {
                cliaddr = it->second;
                sendto(sockfd, (const char *)mesg, strlen(mesg), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
                cout << buffer << " sent.\n" << "IP: " << inet_ntoa(cliaddr.sin_addr)<<":"<< ntohs(cliaddr.sin_port) << endl;
            }
        }
    }

    close(sockfd);
}