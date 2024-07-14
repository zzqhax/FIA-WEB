#include "con.h"
#include "dbm.h"
#include "pch.h"

void handle(int clientfd){
    char buffer[1024];
    int stat = read(clientfd,buffer,sizeof(buffer));
    if(stat > 0){
        std::cout << std::string(buffer,stat) << std::endl;
    }
    else{
        ::close(clientfd);
    }
}

int main(){
    CON server;
    server.create();
    server.bind("127.0.0.1",8081);
    server.listen(10);

    std::cout << "aaaaaaa" << std::endl;
    server.run(handle);
    return 0;
}