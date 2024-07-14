#include "dbm.h"

DBM::DBM(std::string& host, const std::string& user, const std::string& password, const std::string& database, const int port) {
    // mysql init
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(host, user, password);
        con->setSchema(database);
        std::cout << "connect to mysql successful" << std::endl;
    } catch (sql::SQLException &e) {
        std::cerr << "MYSQL ERROR: " << e.what() << std::endl;
    }

    // redis init
    char* redisHost = host.data();
    context = redisConnect(redisHost, port);
    if (context == nullptr || context->err) {
        if (context) {
            std::cerr << "REDIS ERROR: " << context->errstr << std::endl;
        } else {
            std::cerr << "REDIS ERROR: can't allocate redis context" << std::endl;
        }
    } else {
        std::cout << "connect to redis successful" << std::endl;
    }
}

DBM::~DBM() {
    // mysql delete
    if (res) delete res;
    if (stmt) delete stmt;
    if (con) delete con;

    // redis delete
    if (context) redisFree(context);
    if (reply) freeReplyObject(reply);
}

void DBM::DBM_insert(const std::string& username, const std::string& password) {
    int state = 0;
    std::cout << "total insert start" << std::endl;
    state = mysql_insert(username, password);
    if (state == 0) {
        redis_insert(username, password);
    } else {
        std::cout << "insert error" << std::endl;
        return;
    }
    std::cout << "total insert complete" << std::endl;
}

void DBM::DBM_delete(const std::string& username) {
    redis_delete(username);
    mysql_delete(username);
}

std::string DBM::DBM_search(const std::string& username) {
    std::string result = redis_search(username);
    if (result != "$") {
        return result;
    }
    result = mysql_search(username);
    if (result != "$") {
        return result;
    }
    return "$";
}

void DBM::DBM_update(const std::string& username, const std::string& password) {
    redis_delete(username);
    mysql_delete(username);
    DBM_insert(username, password);
    sleep(500);
    redis_delete(username);
}

void DBM::redis_insert(const std::string& username, const std::string& password) {
    std::string query = "SET " + username + " " + password;
    reply = (redisReply*)redisCommand(context, query.data());
    if (!reply) {
        std::cerr << "REDIS ERROR: " << context->errstr << std::endl;
        return;
    }
    std::cout << query << std::endl;
    freeReplyObject(reply);
}

int DBM::mysql_insert(const std::string& username, const std::string& password) {
    try {
        stmt = con->createStatement();
        std::string query = "INSERT INTO info (username, password) VALUES ('" + username + "','" + password + "')";
        std::cout << query << std::endl;
        stmt->execute(query);
        return 0;
    } catch (sql::SQLException& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}

void DBM::redis_delete(const std::string& username) {
    std::string query = "DEL " + username;
    reply = (redisReply*)redisCommand(context, query.data());
    if (!reply) {
        std::cerr << "REDIS ERROR: " << context->errstr << std::endl;
        return;
    }
    std::cout << query << std::endl;
    freeReplyObject(reply);
}

void DBM::mysql_delete(const std::string& username) {
    try {
        stmt = con->createStatement();
        std::string query = "DELETE FROM info WHERE username = '" + username + "'";
        std::cout << query << std::endl;
        stmt->execute(query);
        std::cout << "delete successful" << std::endl;
    } catch (sql::SQLException& e) {
        std::cerr << e.what() << std::endl;
    }
}

std::string DBM::redis_search(const std::string& username) {
    std::string result = "$";
    std::string query = "GET " + username;
    reply = (redisReply*)redisCommand(context, query.data());
    if (!reply) {
        std::cerr << "REDIS ERROR: " << context->errstr << std::endl;
        return result;
    }
    if (reply->type == REDIS_REPLY_STRING) {
        result = reply->str;
    }
    freeReplyObject(reply);
    return result;
}

std::string DBM::mysql_search(const std::string& username) {
    std::string result = "$";
    try {
        stmt = con->createStatement();
        std::string query = "SELECT password FROM info WHERE username = '" + username + "'";
        std::cout << query << std::endl;
        res = stmt->executeQuery(query);
        if (res->next()) {
            result = res->getString("password");
        }
    } catch (sql::SQLException& e) {
        std::cerr << e.what() << std::endl;
    }
    return result;
}

void DBM::redis_TimeOut(const std::string& username, int time) {
    std::string T = std::to_string(time);
    std::string query = "EXPIRE " + username + " " + T;
    reply = (redisReply*)redisCommand(context, query.data());
    if (!reply) {
        std::cerr << "REDIS ERROR: " << context->errstr << std::endl;
        return;
    }
    std::cout << query << std::endl;
    freeReplyObject(reply);
}
