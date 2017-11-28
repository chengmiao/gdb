#pragma once 


/*
 * Feature List: 
 * 1. db connection pool 
 *
 *
 */

#include <vector>
#include <memory>

#include "mysql_connection.h"


#include <iostream>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "fmt/format.h"

#include <functional>
#include "sqlutils.h"
#include "row.hpp"

#include "dbqueue.hpp"

#define MAX_CONN_COUNT 8
struct DBConfig
{
    DBConfig(const char * h = "127.0.0.1", int pt = 3306,const char *u = "root",const char * p  = "",const char  * n = "default")
    {
        strcpy(host,h); 
        port = pt; 
        strcpy(user,u); 
        strcpy(pass,p); 
        strcpy(name,n); 

        connection = nullptr; 
    }

    char name[64];//unique db name  
    char host[64]; 
    int  port; 
    char user[64] ; 
    char pass[64] ; 
    sql::Connection * connection; 
}; 

class TinyDB 
{
    public:
        typedef std::shared_ptr<DBConfig> DBConfigPtr; 
        typedef std::shared_ptr<sql::Connection > ConnectionPtr; 
        typedef std::function<void(sql::ResultSet &) > ResultFunc; 

        TinyDB(const std::string & host ="127.0.0.1",
                int port = 3316,
                const std::string & user = "root",
                const std::string & passwd = "", const std::string& db = "",const std::string &name="default")
        {
            m_configs[name] = DBConfigPtr(new  DBConfig(host.c_str(),port,user.c_str(),passwd.c_str())); 
        }
        void add(const std::string & name, 
                const std::string & host ="127.0.0.1",
                int port = 3306,
                const std::string & user = "root",
                const std::string & passwd = "")
        {
            m_configs[name] = DBConfigPtr(new  DBConfig(host.c_str(),port,user.c_str(),passwd.c_str())); 
        }

        void init(const char * db = nullptr)
        {

            m_driver = get_driver_instance();
            for(auto cfg:m_configs) 
            {
                DBConfigPtr dbCfg = cfg.second; 
                fmt::MemoryWriter addr ;
                addr<<"tcp://"<< dbCfg->host<< ":"<< dbCfg->port; 
                m_conn =  m_driver->connect(addr.c_str(), dbCfg->user, dbCfg->pass) ;
                if (m_conn->isValid())
                {
                    dbCfg->connection = m_conn; 
                    std::cout << "connect to db success" << std::endl; 

                    if (db != nullptr) 
                    {
                        this->db(db); 
                    }
                }
            }

        }

        TinyDB & db(const std::string & dbName)
        {
            if (m_conn) 
            {
                m_conn->setSchema(dbName); 
            }
            return *this; 
        }

        ResultSetUPtr  get(DBQueue & queue){
            sql::Statement * stmt = m_conn->createStatement();
            sql::ResultSet  *res  = stmt->executeQuery(queue.sql()); 

            delete stmt; 
            return ResultSetUPtr(res); 
        }


        void get(DBQueue& queue , ResultFunc func)
        {
            sql::Statement * stmt = m_conn->createStatement();
            sql::ResultSet  *res  = stmt->executeQuery(queue.sql()); 
            func(*res); 
            delete stmt; 
            delete res; 
        }
        void get( ResultFunc func)
        {
            return get(m_queue,func); 
        }

        Row  first(DBQueue & queue ){
            sql::Statement * stmt = m_conn->createStatement();
            sql::ResultSet * res  = stmt->executeQuery(queue.sql()); 
            return Row(ResultSetSPtr(res )); 
        }

        Row  first(){
            return first(m_queue); 
        }




        int execute(const DBQueue & queue)
        {
            std::cout << "exectue:" << queue.sql() << std::endl; 
            sql::Statement *stmt =  m_conn->createStatement();
            int ret = stmt->execute(queue.sql()); 
            delete stmt; 
            return ret; 
        }
        int execute()
        {
            return execute(m_queue); 
        }

        DBQueue & queue()
        {
            return m_queue; 
        }


    private:
        std::map<std::string ,DBConfigPtr> m_configs; 
        DBQueue m_queue ; 
        sql::Driver *m_driver;
        sql::Connection * m_conn; 
        std::vector<std::shared_ptr<sql::Connection > >  m_connPool; 
}; 
