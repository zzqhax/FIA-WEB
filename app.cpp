#include "con.h"
#include "dbm.h"
#include "pch.h"

void sendFile(int clientfd, const std::string& filename, const std::string& contentType) {
    std::ifstream file("src/" + filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::ostringstream content;
    content << file.rdbuf();
    file.close();

    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + std::to_string(content.str().length()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += content.str();

    write(clientfd, response.c_str(), response.length());
}


std::pair<std::string, std::string> parsePostData(const std::string& request) {
    std::string delimiter = "\r\n\r\n";
    std::string::size_type pos = request.find(delimiter);
    std::string body = request.substr(pos + delimiter.length());

    std::string username, password;
    delimiter = "&";
    pos = body.find(delimiter);
    username = body.substr(0, pos);
    password = body.substr(pos + delimiter.length());

    delimiter = "=";
    pos = username.find(delimiter);
    username = username.substr(pos + delimiter.length());

    pos = password.find(delimiter);
    password = password.substr(pos + delimiter.length());

    return {username, password};
}

void handle(int clientfd, DBM& dbm) {
    char buffer[2048];
    int stat = read(clientfd, buffer, sizeof(buffer));
    if (stat > 0) {
        std::string request(buffer, stat);
        std::cout << request << std::endl;

        if (request.find("GET /login") != std::string::npos) {
            sendFile(clientfd, "login.html", "text/html");
        } else if (request.find("GET /register") != std::string::npos) {
            sendFile(clientfd, "register.html", "text/html");
        } else if (request.find("GET /main") != std::string::npos) {
            sendFile(clientfd, "main.html", "text/html");
        } else if (request.find("POST /login") != std::string::npos) {
            auto credentials = parsePostData(request);
            std::string username = credentials.first;
            std::string password = credentials.second;

            std::string storedPassword = dbm.DBM_search(username);
            if (storedPassword != "$" && storedPassword == password) {
                std::string response = "HTTP/1.1 302 Found\r\n";
                response += "Location: /main\r\n";
                response += "Connection: close\r\n";
                response += "\r\n";
                write(clientfd, response.c_str(), response.length());
            } else {
                std::string response = "HTTP/1.1 401 Unauthorized\r\n";
                response += "Content-Type: text/html\r\n";
                response += "Content-Length: 15\r\n";
                response += "Connection: close\r\n";
                response += "\r\n";
                response += "Login Failed";
                write(clientfd, response.c_str(), response.length());
            }
        } else if (request.find("POST /register") != std::string::npos) {
            auto credentials = parsePostData(request);
            std::string username = credentials.first;
            std::string password = credentials.second;

            dbm.DBM_insert(username, password);

            std::string response = "HTTP/1.1 302 Found\r\n";
            response += "Location: /login\r\n";
            response += "Connection: close\r\n";
            response += "\r\n";
            write(clientfd, response.c_str(), response.length());
        } else {
            std::string notFoundResponse = "HTTP/1.1 404 Not Found\r\n";
            notFoundResponse += "Content-Type: text/html\r\n";
            notFoundResponse += "Content-Length: 13\r\n";
            notFoundResponse += "Connection: close\r\n";
            notFoundResponse += "\r\n";
            notFoundResponse += "404 Not Found";

            write(clientfd, notFoundResponse.c_str(), notFoundResponse.length());
        }
        ::close(clientfd);
    } else {
        ::close(clientfd);
    }
}

int main() {
    std::string mysqlHost = "localhost"; 
    std::string mysqlUser = "root";      
    std::string mysqlPassword = "95137?Abc"; 
    std::string mysqlDatabase = "Fia_ID_Password"; 
    int redisPort = 6379;                

    DBM dbm(mysqlHost, mysqlUser, mysqlPassword, mysqlDatabase, redisPort);

    CON server;
    server.create();
    server.bind("127.0.0.1", 8081);
    server.listen(10);

    std::cout << "Server started on 127.0.0.1:8081" << std::endl;
    server.run([&dbm](int clientfd) { handle(clientfd, dbm); });
    return 0;
}
