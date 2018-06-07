#pragma once 


/*
 * Feature List: 
 * 1. db connection pool 
 *
 *
 */

#include <vector>
#include <memory>


#include "mysql/mysql.h"

#include <iostream>
#include <map>

#include "fmt/format.h"

#include <functional>
//#include "sqlutils.h"
#include "row.hpp"

#include "dbquery.hpp"
#include "dbconnection.hpp"

namespace gdp 
{
    namespace db
    {
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
	    DBConnectionPtr   connection; 
	    int pool_size; 
	}; 

	class GDb 
	{
	    public:
		typedef std::function<void(ResultSetPtr ) > ResultHandler; 

		GDb(const std::string & host ="127.0.0.1",
			int port = 3316,
			const std::string & user = "root",
			const std::string & passwd = "", const std::string& db = "",const std::string &name="default")
		{
		    m_configs[name] =  DBConfig(host,port,user,passwd,name); 
		}
		void add(
			const std::string & host ="127.0.0.1",
			int port = 3306,
			const std::string & user = "root",
			const std::string & passwd = "",
			const std::string & name= "default"
			)
		{
		    m_configs[name] =  DBConfig(host,port,user,passwd,name); 
		}

		void init(const std::string & db){
		    m_db = db;
		    connect();   
		}

		void connect() {
		    for(auto && cfg:m_configs) 
		    {
			DBConfig&  dbInfo = cfg.second; 
			if (dbInfo.name == "default" )
			{
			    m_default = cfg.second; 
			}

			DBConnectionPtr conn = std::make_shared<DBConnection>(); 
			conn->init(m_db,dbInfo.user, dbInfo.pass,dbInfo.host,dbInfo.port); 
			if (conn->is_connected())
			{
			    dbInfo.connection = conn; 
			    std::cout << "connect to db success" << std::endl; 
			}
			else 
			{
			    std::cout << "connect to db failed" << std::endl; 
			}
		    }

		}

		GDb & usedb(const std::string & dbName)
		{
		    m_default.connection->use(dbName); 
		    return *this; 
		}

		ResultSetPtr  get(DBQuery & query){
		    if (!is_valid()) {
			connect();
		    }

		    if (is_valid())
		    {
			return m_default.connection->query(query.sql()); 
		    }
		    return nullptr; 
		}

		bool is_valid()
		{
		    if (m_default.connection )
		    {
			return m_default.connection->is_connected(); 
		    }

		    return false; 
		}


		void get(DBQuery& query , ResultHandler func)
		{
		    if (!is_valid()) {
			connect();
		    }
		    if (is_valid())
		    {
			ResultSetPtr result = m_default.connection->query(query.sql()); 
			func(result ); 
		    }
		}
		void get( ResultHandler func)
		{
		    return get(m_query,func); 
		}

		Row  first(DBQuery & query ){
		    if (!is_valid()) {
			connect();
		    }

		    if (is_valid())
		    {
			ResultSetPtr res = m_default.connection->query(query.sql()); 
			return Row::first(res); 
		    }
		    return Row(); 
		}

		Row  first(){
		    return first(m_query); 
		}

		template<typename ... Args>
		    bool execute(const char* format, const Args & ... args ) {
			fmt::MemoryWriter statement;
			statement.write(format, args...);
			if (!is_valid()) {
			    connect();
			}

			if (is_valid())
			{
			    std::cout << "exectue:" << statement.c_str() << std::endl; 
			    return m_default.connection->execute(statement.c_str()); 
			}
			return false; 
		    }

		bool execute(const DBQuery & query)
		{
		    if (!is_valid()) {
			connect();
		    }
		    if (is_valid())
		    {
			std::cout << "exectue:" << query.sql() << std::endl; 
			m_default.connection->execute(query.sql()); 
		    }
		    return false; 
		}

		int execute()
		{
		    return execute(m_query); 
		}

		DBQuery & query()
		{
		    return m_query; 
		}

	    private:
		std::map<std::string ,DBConfig> m_configs; 
		DBQuery m_query ; 
		std::string m_db;
		DBConfig m_default; 
	}; 
    }
}
