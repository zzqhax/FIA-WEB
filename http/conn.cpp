#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include "config.h"

#define BUFFERSIZE 1024

int main() {
    MYSQL *conn = mysql_init(nullptr);
    if (!conn) {
        std::cerr << "MySQL initialization failed" << std::endl;
        exit(1);
    }

    // 设置 MySQL 连接参数
    if (!mysql_real_connect(conn, "localhost", "root", "95137?Abc", "FiaWeb", 0, nullptr, 0)) {
        std::cerr << "MySQL connection failed: " << mysql_error(conn) << std::endl;
        exit(1);
    }

    int listenfd = 0;
    int stat = 0;
    struct sockaddr_in servaddr;
    int addrlen = sizeof(servaddr);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        std::cerr << "Socket error" << std::endl;
        exit(1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    stat = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (stat == -1) {
        std::cerr << "Bind error" << std::endl;
        exit(1);
    }

    stat = listen(listenfd, 3);
    if (stat == -1) {
        std::cerr << "Listen error" << std::endl;
        exit(1);
    }
    std::cout << "Server is running and waiting for connections..." << std::endl;

    while (1) {
        int servfd = accept(listenfd, (struct sockaddr *)&servaddr, (socklen_t*)&addrlen);
        if (servfd == -1) {
            std::cerr << "Accept error" << std::endl;
            continue;
        }

        char buffer[BUFFERSIZE] = {0};
        stat = read(servfd, buffer, BUFFERSIZE);
        if (stat == -1) {
            std::cerr << "Read error" << std::endl;
            close(servfd);
            continue;
        }
        std::cout << "Received request: " << buffer << std::endl;

        std::string request(buffer);
        size_t user_pos = request.find("__user=");
        size_t pass_pos = request.find("__pass=");

        if (user_pos != std::string::npos && pass_pos != std::string::npos) {
            size_t user_end = request.find("&", user_pos);
            std::string user_value = request.substr(user_pos + 7, user_end - user_pos - 7);
            std::string pass_value = request.substr(pass_pos + 7);
            size_t pass_end = pass_value.find(" "); 

            if (pass_end != std::string::npos) {
                pass_value = pass_value.substr(0, pass_end);
            }

            std::cout << "Username: " << user_value << std::endl;
            std::cout << "Password: " << pass_value << std::endl;

            // 插入数据到数据库
            std::string insert_query = "INSERT INTO user (username, password) VALUES ('" + user_value + "', '" + pass_value + "')";
            if (mysql_query(conn, insert_query.c_str())) {
                std::cerr << "Insert error: " << mysql_error(conn) << std::endl;
            } else {
                std::cout << "User data inserted successfully." << std::endl;
            }
        } else {
            std::cout << "Parameters not found." << std::endl;
        }

        std::ifstream file("hello.html");
        if (!file) {
            std::cerr << "Error opening hello.html" << std::endl;
            close(servfd);
            continue;
        }
        std::stringstream ss;
        ss << file.rdbuf();
        std::string html_content = ss.str();

        std::string http_response = "HTTP/1.1 200 OK\r\n";
        http_response += "Content-Type: text/html\r\n";
        http_response += "Content-Length: " + std::to_string(html_content.length()) + "\r\n";
        http_response += "\r\n";
        http_response += html_content;

        stat = write(servfd, http_response.c_str(), http_response.length());
        if (stat == -1) {
            std::cerr << "Write error" << std::endl;
        }

        close(servfd);
    }
    close(listenfd);
    mysql_close(conn);
    return 0;
}


