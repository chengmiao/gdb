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
    DBConfig(const std::string & h = "127.0.0.1", 
            int pt = 3306,
            const std::string & u = "root",
            const std::string & p = "",
            const std::string & n = "default")
    {
        name = n; 
        host = h; 
        port =pt; 

        user = u; 
        pass = p; 

        pool_size = 1; 
        connection = nullptr; 
    }

    std::string name; 
    std::string host;
    int port; 
    std::string user; 
    std::string pass; 
    sql::Connection * connection; 
    int pool_size; 
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
            m_configs[name] = DBConfigPtr(new  DBConfig(host,port,user,passwd,name)); 
        }
        void add(const std::string & name, 
                const std::string & host ="127.0.0.1",
                int port = 3306,
                const std::string & user = "root",
                const std::string & passwd = "")
        {
            m_configs[name] = DBConfigPtr(new  DBConfig(host,port,user,passwd,name)); 
        }

        void init(const char * db = nullptr)
        {
            m_driver = get_driver_instance();
            for(auto cfg:m_configs) 
            {
                DBConfigPtr dbInfo = cfg.second; 
                if (dbInfo->name == "default" )
                {
                    m_default = dbInfo; 
                }
                fmt::MemoryWriter addr ;
                addr<<"tcp://"<< dbInfo->host<< ":"<< dbInfo->port; 
                sql::Connection * conn =  m_driver->connect(addr.c_str(), dbInfo->user, dbInfo->pass) ;
                if (conn->isValid())
                {
                    dbInfo->connection = conn; 
                    std::cout << "connect to db success" << std::endl; 

                    if (db != nullptr) 
                    {
                        this->usedb(db); 
                    }
                }
            }

        }

        TinyDB & usedb(const std::string & dbName)
        {
            if (m_default) 
            {
                m_default->connection->setSchema(dbName); 
            }
            return *this; 
        }

        ResultSetUPtr  get(DBQueue & queue){
            sql::Statement * stmt = m_default->connection->createStatement();
            sql::ResultSet  *res  = stmt->executeQuery(queue.sql()); 

            delete stmt; 
            return ResultSetUPtr(res); 
        }


        void get(DBQueue& queue , ResultFunc func)
        {
            sql::Statement * stmt = m_default->connection->createStatement();
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
            sql::Statement * stmt = m_default->connection->createStatement();
            sql::ResultSet * res  = stmt->executeQuery(queue.sql()); 
            return Row(ResultSetSPtr(res )); 
        }

        Row  first(){
            return first(m_queue); 
        }




        int execute(const DBQueue & queue)
        {
            std::cout << "exectue:" << queue.sql() << std::endl; 
            sql::Statement *stmt =  m_default->connection->createStatement();
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
        DBConfigPtr m_default; 
        std::vector<std::shared_ptr<sql::Connection > >  m_connPool; 
}; 
