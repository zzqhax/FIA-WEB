#ifndef DBM_H
#define DBM_H

#include <iostream>
#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <hiredis/hiredis.h>

class DBM {
public:
    DBM(std::string& host, const std::string& user, const std::string& password, const std::string& database, const int port);
    ~DBM();

    void DBM_insert(const std::string& username, const std::string& password);
    void DBM_delete(const std::string& username);
    std::string DBM_search(const std::string& username);
    void DBM_update(const std::string& username, const std::string& password);

private:
    void redis_insert(const std::string& username, const std::string& password);
    int mysql_insert(const std::string& username, const std::string& password);
    void redis_delete(const std::string& username);
    void mysql_delete(const std::string& username);
    std::string redis_search(const std::string& username);
    std::string mysql_search(const std::string& username);
    void redis_TimeOut(const std::string& username, int time);

    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    redisContext *context;
    redisReply *reply;
};

#endif // DBM_H