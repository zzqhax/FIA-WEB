//dbm.h

#include "pch.h"

class DBM{
    public:
    DBM(std::string& host,const std::string& user,const std::string& password,const std::string& database,const int port){
        //mysql init
        try{
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(host,user,password);
            con->setSchema(database);
            std::cout << "connect to mysql successful" << std::endl;
        }
        catch(sql::SQLException &e){
            std::cerr << "MYSQL ERROR (line 32): " << e.what() << std::endl;
        }

        //redis init
        char* redisHost = host.data();
        context = redisConnect(redisHost,port);
        std::cout << "connect to redis successful" << std::endl;
    }

    ~DBM(){
        //mysql delete
        delete res;
        delete con;
        delete stmt;
        //redis delete
        delete context;
        delete reply;
    }

    void DBM_insert(const std::string& username,const std::string& password){
        int state = 0;
        std::cout << "total insert start" << std::endl;
        state = mysql_insert(username,password);
        if(state == 0){
            redis_insert(username,password);
        }
        else{
            std::cout << "insert error" << std::endl;
            return;
        }
        std::cout << "total insert complete" << std::endl;
    }
    
    void DBM_delete(const std::string& username){
        redis_delete(username);
        mysql_delete(username);
    }
    
    std::string DBM_search(const std::string& username){    
        std::string result = redis_search(username);
        if(result != "$"){
            return result;
        }
        result = mysql_search(username);
        if(result != "$"){
            return result;
        }
        return "$";
    }    
    
    //total update
    void DBM_update(const std::string& username,const std::string& password){
        redis_delete(username);
        mysql_delete(username);
        DBM_insert(username,password);
        sleep(500);
        redis_delete(username);
    }

    private:
    //redis insert
    void redis_insert(const std::string& username,const std::string& password){
        std::string query = "SET " + username + " "+ password;
        reply = (redisReply*)redisCommand(context,query.data());

        std::cout << query << std::endl;
        
        freeReplyObject(reply);
    }

    //mysql insert
    int mysql_insert(const std::string& username,const std::string& password){
        try{
            stmt = con->createStatement();
            std::string query = "insert into info (username,password) values ('" + username + "','" + password + "')";
            std::cout << query << std::endl;
            stmt -> execute(query);
            return 0;
        }
        catch(sql::SQLException& e){
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }

    //redis delete
    void redis_delete(const std::string& username){
        std::string query = "DEL " + username;
        reply = (redisReply*)redisCommand(context,query.data());
        
        std::cout << query << std::endl;
        
        freeReplyObject(reply);
    }

    //mysql delete
    void mysql_delete(const std::string& username){
        try{
            stmt = con->createStatement();
            std::string query = "DELETE FROM info WHERE username = '" + username +"'";
            std::cout << query << std::endl;
            stmt->execute(query);
            std::cout << "delete successful" << std::endl;
        }
        catch(sql::SQLException& e){
            std::cerr << e.what() << std::endl;
        }
    }

    //redis search
    std::string redis_search(const std::string& username){
        std::string result ="$";
        std::string query = "GET " + username;
        if((redisReply*)redisCommand(context,query.data())){
            reply = (redisReply*)redisCommand(context,query.data());
        }
        result = reply->str;
        freeReplyObject(reply);
        return result;        
    }

    //mysql search
    std::string mysql_search(const std::string& username){
        std::string result = "$";
        stmt = con->createStatement();
        std::string query = "SELECT password FROM info WHERE username = '" + username + "'";
        std::cout << query << std::endl;
        res = stmt->executeQuery(query);

        if (res->next()) {
            result = res->getString("password");
        }       
        return result;
    }

    //redis TimeOut
    void redis_TimeOut(const std::string& username,int time){
        std::string T = std::to_string(time);
        std::string query = "EXPIRE " + username + " " + T;
        reply = (redisReply*)redisCommand(context,query.data());
        
        std::cout << query << std::endl;
        freeReplyObject(reply);
    }

    
    private:
    //mysql starter
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    //redis starter
    redisContext *context;
    redisReply *reply;
};


