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


#define MAX_CONN_COUNT 8

class TinyDB 
{
    public:
        typedef std::shared_ptr<sql::Connection > ConnectionPtr; 
        typedef std::function<void(sql::ResultSet &) > ResultFunc; 

        TinyDB(const std::string & host,int port ,const std::string & user,const std::string & passwd)
        {
            m_driver = get_driver_instance();
            //for(int i=0;i < MAX_CONN_COUNT ;i ++)
            {
                m_conn =  m_driver->connect("tcp://127.0.0.1:3306", user.c_str(), passwd.c_str()) ;
                if (m_conn->isValid())
                {
                    std::cout << "connect to db success" << std::endl; 
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

        template <typename ... Args>
            TinyDB & select(const Args & ... args) 
            {
                int argLen = sizeof ...(Args); 
                m_sql << "select " ; 

                fmt::MemoryWriter format; 
                for (int i  = 0; i   < argLen  ; i++)
                {
                    if (i < argLen -1 )
                    {
                        format<< " {}, "; 
                    }
                    else {
                        format<< " {} "; 
                    }
                }
                //std::cout << "format is " << format.c_str() << std::endl; 
                m_sql.write(format.c_str(),args...); 
                m_sql.write(" from {} " , m_table); 
                return *this; 
            }
        TinyDB & select(const std::string & selData)
        {
            m_sql.write("select {} from {} ",selData,m_table ); 
            return *this; 
        }

        TinyDB & table(const std::string & tbName)
        {
            m_table = tbName; 
            return *this; 
        }

        TinyDB & where(const std::string & key , const std::string & op, const char * term )
        {
            return where(key,op,std::string(term)); 
        }
        TinyDB & where(const std::string & key , const std::string & op, const std::string & term)
        {
            fmt::MemoryWriter termStr; 
            termStr.write("{} {} \"{}\"", key , op, term);  
            wheres.push_back(termStr.c_str()); 

            m_sql.write( wheres.size() > 1 ?" and {} ":" where {} " , termStr.c_str()); 
            return *this; 
        }
        template <typename T> 
            TinyDB & where(T term)
            {
                fmt::MemoryWriter termStr; 
                termStr.write(" {} ",  term);  
                wheres.push_back(termStr.c_str()); 

                m_sql.write( wheres.size() > 1 ?" and  {} ":"where {} " , term ); 
                return *this; 
            }
        template <typename T> 
        TinyDB & where(const std::string & key , const std::string & op, T   term)
        {
            fmt::MemoryWriter termStr; 
            termStr.write(" {} {} {} ", key , op, term);  
            wheres.push_back(termStr.c_str()); 
            //ugly way to do this 
            m_sql.write( wheres.size() > 1 ?" and {} ":"where {} " , termStr.c_str()); 
            return *this; 
        }

        template<typename T> 
        TinyDB & or_where(const std::string & key ,const std::string & op , T  term)
        {
            fmt::MemoryWriter termStr; 
            termStr.write(" {} {} {} ", key , op, term);  
            wheres.push_back(termStr.c_str()); 
            m_sql.write( " or {} ", termStr.c_str()); 
            return *this; 
        }

        TinyDB & limit( unsigned int count)
        {

            m_sql.write(" limit {} " ,count); 

            return *this; 
        }
        TinyDB & limit(unsigned int offset , unsigned int count)
        {

            m_sql.write(" limit {}, {}  " ,offset , count); 
            return *this; 
        }
        TinyDB & group_by (const std::string & col)
        {
            m_sql.write(" group by {} " ,col); 
            return *this; 
        }


        TinyDB & order_by(const std::string & col, const std::string & type = "asc" )
        {
            m_sql.write(" order by {} {} " ,col,type ); 
            return *this; 
        }

        TinyDB & join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
        {
            return left_join(tb,lId,op,rId); 
        }

        TinyDB & left_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
        {
            m_sql.write(" left join {} on  {} {}  {} " ,tb, lId,op,rId); 
            return *this; 
        }

        TinyDB & right_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
        {
            m_sql.write(" left join {} on  {} {}  {} " ,tb, lId,op,rId); 
            return *this; 
        }

        ResultSetUPtr  get(){
            sql::Statement * stmt = m_conn->createStatement();
            sql::ResultSet  *res  = stmt->executeQuery(m_sql.c_str()); 

            delete stmt; 
            return ResultSetUPtr(res); 
        }

        
        void get(ResultFunc func)
        {
            sql::Statement * stmt = m_conn->createStatement();
            sql::ResultSet  *res  = stmt->executeQuery(m_sql.c_str()); 
            func(*res); 
            delete stmt; 
            delete res; 
        }
        
        Row  first(){
            sql::Statement * stmt = m_conn->createStatement();
            sql::ResultSet * res  = stmt->executeQuery(m_sql.c_str()); 
            return Row(ResultSetSPtr(res )); 
        }


        const char *  sql()
        {
            return m_sql.c_str(); 
        }


        int execute(const std::string & sql)
        {
            sql::Statement *stmt =  m_conn->createStatement();
            int ret = stmt->execute(m_sql.c_str()); 
            delete stmt; 
            return ret; 
        }


    private:
        
        sql::Driver *m_driver;
        sql::Connection * m_conn; 

        std::vector<std::string > wheres; 
        std::string m_db; 
        std::string  m_table; 
        std::vector<std::shared_ptr<sql::Connection > >  m_connPool; 

        fmt::MemoryWriter m_sql ;
}; 
