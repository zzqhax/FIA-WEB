#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <hiredis/hiredis.h>
#include <algorithm>

class DBM{
    private:
    //mysql starter
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    //redis starter
    redisContext *context;
    redisReply *reply;

    public:
    DBM(std::string& host,const std::string& user,const std::string& password,const std::string& database,const int port){
        //mysql init
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(host,user,password);
        con->setSchema(database);
        std::cout << "connect to mysql successful" << std::endl;

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
    //total insert
    void DBM_insert(const std::string& username,const std::string& password){
        std::cout << "total insert start" << std::endl;
        mysql_insert(username,password);
        redis_insert(username,password);
        std::cout << "total insert complete" << std::endl;
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
    void mysql_insert(const std::string& username,const std::string& password){
        stmt = con->createStatement();
        std::string query = "insert into info (username,password) values ('" + username + "','" + password + "')";
        std::cout << query << std::endl;
        stmt -> execute(query);
        std::cout << "insert successful" << std::endl;
    }

    private:
    //redis delete
    void redis_delete(const std::string& username){
        std::string query = "DEL " + username;
        reply = (redisReply*)redisCommand(context,query.data());

        std::cout << query << std::endl;
        
        freeReplyObject(reply);
    }

    //mysql delete
    void mysql_delete(const std::string& username){
        stmt = con->createStatement();
        std::string query = "DELETE FROM info WHERE username = '" + username +"'";
        std::cout << query << std::endl;
        stmt->execute(query);
        std::cout << "delete successful" << std::endl;
    }

    public:
    void DBM_delete(const std::string& username){
        redis_delete(username);
        mysql_delete(username);
    }

    //redis TimeOut
    void redis_TimeOut(const std::string& username,int time){
        std::string T = std::to_string(time);
        std::string query = "EXPIRE " + username + " " + T;
        reply = (redisReply*)redisCommand(context,query.data());
        
        std::cout << query << std::endl;
        freeReplyObject(reply);
    }

    //total update
    void DBM_update(const std::string& username,const std::string& password){
        redis_delete(username);
        mysql_delete(username);
        DBM_insert(username,password);
        sleep(500);
        redis_delete(username);
    }

    //redis search
    private:
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

    //total search
    public:
    std::string search(const std::string& username){    
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
};

int main(){
    std::cout << "start";
    std::string host = "127.0.0.1";
    std::string user = "root";
    std::string password = "95137?Abc";
    std::string database = "Fia_ID_Password";
    int port = 6379;
    auto DB = new DBM(host,user,password,database,port);

    int t = 0;
    std::cin >> t;
    while(t--){
        int op = 0;
        std::cin >> op;
        std::string User;
        std::string Pass;
        //insert
        if(op == 1){
            std::cin >> User;
            std::cin >> Pass;
            DB->DBM_insert(User,Pass);
        }
        //delete
        else if(op == 2){
            std::cin >> User;
            DB->DBM_delete(User);
        }
        else if(op == 3){
            std::cin >> User;
            std::string res = DB->search(User);
            std::cout << res << std::endl;
        }
        else{
            std::cin >> User;
            std::cin >> Pass;
            DB->DBM_update(User,Pass);
        }
    }
    return 0;
}