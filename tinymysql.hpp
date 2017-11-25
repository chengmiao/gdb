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

class TinyMysql 
{
    public:
        typedef std::shared_ptr<sql::Connection > ConnectionPtr; 
        typedef std::function<void(sql::ResultSet &) > ResultFunc; 

        TinyMysql(const std::string & host,int port ,const std::string & user,const std::string & passwd, const std::string& dbName = "")
        {
            m_driver = get_driver_instance();
            //for(int i=0;i < MAX_CONN_COUNT ;i ++)
            {
                m_conn =  m_driver->connect("tcp://127.0.0.1:3306", user.c_str(), passwd.c_str()) ;
                if (m_conn->isValid())
                {
                    std::cout << "connect to db success" << std::endl; 

		    if (!dbName.empty()) 
		    {
			this->db(dbName); 
		    }
                }
            }
        }
        TinyMysql & db(const std::string & dbName)
        {
            m_db = dbName; 
            if (m_conn) 
            {
                m_conn->setSchema(dbName); 
            }
            return *this; 
        }

        template <typename ... Args>
            TinyMysql & insert(const Args & ... args) 
            {
		clear(); 
		int argLen = sizeof ...(Args); 
		m_sql << " insert into " << m_table ; 

		if (argLen > 0 )
		{
		    m_sql << "( "; 
		}
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
		if (argLen > 0)
		{
		    m_sql.write(" )  " ); 
		}

		return *this; 
	    }

        template <typename ... Args>
            TinyMysql & update (const Args & ... args) 
            {
		clear(); 
		int argLen = sizeof ...(Args); 
		m_sql << " update " << m_table ; 

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

		return *this; 
	    }



	template<class T> 
	TinyMysql& set(const std::string & key, T val )
	{
	    fmt::MemoryWriter termStr; 
	    termStr.write(" {} = {} ", key ,  val);  
            sets.push_back(termStr.c_str()); 
	    if (sets.size() <= 1)
	    {
		m_sql << " set " << termStr.c_str(); 
	    }
	    else {
		m_sql << " ," << termStr.c_str(); 
	    }

	    return *this; 
	}

	TinyMysql & del()
	{
	    clear(); 
	    m_sql << "delete  from " << m_table  ; 
	    return *this; 
	}

	TinyMysql& set(const std::string & key, const char *  val)
	{
	    return this->set(key,std::string(val)); 
	}
	TinyMysql& set(const std::string & key, const std::string & val)
	{
	    fmt::MemoryWriter termStr; 
	    termStr.write(" {} = \"{}\" ", key ,  val);  
            sets.push_back(termStr.c_str()); 
	    if (sets.size() <= 1)
	    {
		m_sql << " set " << termStr.c_str(); 
	    }
	    else {
		m_sql << " ," << termStr.c_str(); 
	    }

	    return *this; 
	}
	
	template <class T>
	    std::string printarg(T t)
	    {
		fmt::MemoryWriter val ;
		val << t ; 
		return val.c_str() ; 
	    }
		
	    

	    std::string printarg(const char *  t)
	    {
		return std::string("\"") +t +"\""; 
	    }
	
	    std::string printarg(const std::string& t )
	    {
		return std::string("\"") +t +"\""; 
	    }



        template <typename ... Args>
            TinyMysql & values(const Args & ... args) 
            {
		int argLen = sizeof ...(Args); 
		m_sql << " values " << " ( "  ; 

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
		m_sql.write(format.c_str(),printarg(args)...); 
		m_sql.write(" )  " ); 
		return *this; 
	    }



        template <typename ... Args>
            TinyMysql & select(const Args & ... args) 
            {
		clear(); 
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
        TinyMysql & select(const std::string & selData)
        {
		clear(); 
            m_sql.write("select {} from {} ",selData,m_table ); 
            return *this; 
        }

        TinyMysql & table(const std::string & tbName)
        {
            m_table = tbName; 
            return *this; 
        }

        TinyMysql & where(const std::string & key ,  const char * term )
	{
	    return this->where(key,"=",term); 
	}
        TinyMysql & where(const std::string & key , const std::string & op, const char * term )
        {
            return where(key,op,std::string(term)); 
        }
        TinyMysql & where(const std::string & key , const std::string & op, const std::string & term)
        {
            fmt::MemoryWriter termStr; 
            termStr.write("{} {} \"{}\"", key , op, term);  
            wheres.push_back(termStr.c_str()); 

            m_sql.write( wheres.size() > 1 ?" and {} ":" where {} " , termStr.c_str()); 
            return *this; 
        }
        template <typename T> 
            TinyMysql & where(T term)
            {
                fmt::MemoryWriter termStr; 
                termStr.write(" {} ",  term);  
                wheres.push_back(termStr.c_str()); 

                m_sql.write( wheres.size() > 1 ?" and  {} ":"where {} " , term ); 
                return *this; 
            }


        template <typename T> 
        TinyMysql & where(const std::string & key ,  T   term)
	{
	    return where<T>(key,"=",term); 
	}

        template <typename T> 
        TinyMysql & where(const std::string & key , const std::string & op, T   term)
        {
            fmt::MemoryWriter termStr; 
            termStr.write(" {} {} {} ", key , op, term);  
            wheres.push_back(termStr.c_str()); 
            //ugly way to do this 
            m_sql.write( wheres.size() > 1 ?" and {} ":"where {} " , termStr.c_str()); 
            return *this; 
        }

        template<typename T> 
        TinyMysql & or_where(const std::string & key ,const std::string & op , T  term)
        {
            fmt::MemoryWriter termStr; 
            termStr.write(" {} {} {} ", key , op, term);  
            wheres.push_back(termStr.c_str()); 
            m_sql.write( " or {} ", termStr.c_str()); 
            return *this; 
        }

        TinyMysql & limit( unsigned int count)
        {
            m_sql.write(" limit {} " ,count); 

            return *this; 
        }
        TinyMysql & limit(unsigned int offset , unsigned int count)
        {

            m_sql.write(" limit {}, {}  " ,offset , count); 
            return *this; 
        }
        TinyMysql & group_by (const std::string & col)
        {
            m_sql.write(" group by {} " ,col); 
            return *this; 
        }


        TinyMysql & order_by(const std::string & col, const std::string & type = "asc" )
        {
            m_sql.write(" order by {} {} " ,col,type ); 
            return *this; 
        }

        TinyMysql & join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
        {
            return left_join(tb,lId,op,rId); 
        }

        TinyMysql & left_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
        {
            m_sql.write(" left join {} on  {} {}  {} " ,tb, lId,op,rId); 
            return *this; 
        }

        TinyMysql & right_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
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

        int execute()
        {
	    std::cout << "exectue:" << sql() << std::endl; 
            sql::Statement *stmt =  m_conn->createStatement();
            int ret = stmt->execute(m_sql.c_str()); 
            delete stmt; 
            return ret; 
        }



	void clear()
	{
	    m_sql.clear(); 
	    wheres.clear(); 
	    sets.clear(); 
	}


    private:
        
        sql::Driver *m_driver;
        sql::Connection * m_conn; 

        std::vector<std::string > wheres; 
        std::vector<std::string > sets ; 
        std::string m_db; 
        std::string  m_table; 
        std::vector<std::shared_ptr<sql::Connection > >  m_connPool; 

        fmt::MemoryWriter m_sql ;
}; 
