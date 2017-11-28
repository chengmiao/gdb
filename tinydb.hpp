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
    DBConfig(const char * h, int pt,const char *u,const char * p )
    {
        strcpy(host,h); 
        port = pt; 
        strcpy(user,u); 
        strcpy(pass,p); 

    }

    char host[64]; 
    int  port; 
    char user[64] ; 
    char pass[64] ; 
}; 

class TinyDB 
{
    public:
        typedef std::shared_ptr<sql::Connection > ConnectionPtr; 
        typedef std::function<void(sql::ResultSet &) > ResultFunc; 

        TinyDB(const std::string & host ="127.0.0.1",
                int port = 3316,
                const std::string & user = "root",
                const std::string & passwd = "", const std::string& db = "")
        {

            m_configs.push_back(DBConfig(host.c_str(),port,user.c_str(),passwd.c_str())); 
        }

        void init(const char * db = nullptr)
        {

            m_driver = get_driver_instance();
            for(auto cfg:m_configs) 
            {
                fmt::MemoryWriter addr ;
                addr<<"tcp://"<< cfg.host<< ":"<< cfg.port; 
                m_conn =  m_driver->connect(addr.c_str(), cfg.user, cfg.pass) ;
                if (m_conn->isValid())
                {
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
            m_db = dbName; 
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
        std::vector<DBConfig> m_configs; 
        DBQueue m_queue ; 
        sql::Driver *m_driver;
        sql::Connection * m_conn; 
        std::string m_db; 
        std::vector<std::shared_ptr<sql::Connection > >  m_connPool; 
}; 
