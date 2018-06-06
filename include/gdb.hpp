#pragma once 


/*
 * Feature List: 
 * 1. db connection pool 
 *
 *
 */

#include <vector>
#include <memory>


#include "mysql.h"

#include <iostream>

#include "fmt/format.h"

#include <functional>
#include "sqlutils.h"
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
				typedef std::shared_ptr<DBConfig> DBConfigPtr; 
				typedef std::function<void(ResultSet &) > ResultHandler; 

				GDb(const std::string & host ="127.0.0.1",
						int port = 3316,
						const std::string & user = "root",
						const std::string & passwd = "", const std::string& db = "",const std::string &name="default")
				{
					m_configs[name] = DBConfigPtr(new  DBConfig(host,port,user,passwd,name)); 
				}
				void add(
						const std::string & host ="127.0.0.1",
						int port = 3306,
						const std::string & user = "root",
						const std::string & passwd = "",
						const std::string & name= "default"
						)
				{
					m_configs[name] = DBConfigPtr(new  DBConfig(host,port,user,passwd,name)); 
				}

				void init(const char * db = nullptr){
					if (db != nullptr) {
						m_db = db;
					}
					connect();   
				}

				void connect() {
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
						//sql::Connection * conn =  m_driver->connect(addr.c_str(), dbInfo->user, dbInfo->pass) ;
						DBConnectionPtr conn = std::make_shared<DBConnection>(); 
						if (conn->is_connected())
						{
							dbInfo->connection = conn; 
							std::cout << "connect to db success" << std::endl; 

							if (!m_db.empty()) 
							{
								this->usedb(m_db); 
							}
						}
					}

				}

				GDb & usedb(const std::string & dbName)
				{
					if (m_default) 
					{
						try {
							m_default->connection->setSchema(dbName); 
						}
						catch (const sql::SQLException & e)
						{
							std::cout << " exception :" << e.what() << std::endl; 
						}
					}
					return *this; 
				}

				ResultSetPtr  get(DBQuery & query){
					if (!is_valid()) {
						connect();
					}
					if (is_valid())
					{
						return m_default->connection->query(query.sql()); 
					}
					return nullptr; 
				}

				bool is_valid()
				{
					if (m_default && m_default->connection != nullptr)
					{
						return m_default->connection->is_connected(); 
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
						ResultSetPtr result = m_default->connection->query(query.sql()); 
						func(result ); 
					}
				}
				void get( ResultHandler func)
				{
					return get(m_queue,func); 
				}

				Row  first(DBQuery & query ){
					if (!is_valid()) {
						connect();
					}

					if (is_valid())
					{
						m_default->connection->query(query.sql()); 
					}
					return Row(nullptr); 
				}

				Row  first(){
					return first(m_queue); 
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
							m_default->connection->execute(statement.c_str()); 
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
						m_default->connection->execute(query.sql()); 
					}
					return false; 
				}
				int execute()
				{
					return execute(m_queue); 
				}

				DBQuery & query()
				{
					return m_queue; 
				}


			private:
				std::map<std::string ,DBConfigPtr> m_configs; 
				DBQuery m_queue ; 
				std::string m_db;
				DBConfigPtr m_default; 
				MYSQL *m_mysql;
				std::vector<std::shared_ptr<sql::Connection > >  m_connPool; 
		}; 
	}
}
