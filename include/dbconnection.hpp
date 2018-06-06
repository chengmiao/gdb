#pragma once 

#include "mysql.h"
#include <memory>

class DBConnection{

	public:

		DBConnection(const std::string& host , const std::string & user )
		{

		}
		void init()
		{
			mysql_init(m_mysql);
			my_bool reconnect = 1;
			mysql_options(this->m_mysql, MYSQL_OPT_RECONNECT, &reconnect);
		}
		bool is_connected()
		{
			return m_connected; 
		}

		//execute sql without result 
		int  execute(const std::string & sql ){
 
			int ret = mysql_real_query(m_mysql,sql.c_str(),sql.size() ); 

			return ret ; 
		}

		ResultSetPtr query(const std::string & sql) 
		{
			int ret = mysql_real_query(m_mysql,sql.c_str(),sql.size() ); 
			MYSQL_RES *res = mysql_store_result(m_mysql);
			ResultSetPtr  rset  = std::make_shared<ResultRet>(res); 
			return rset; 
		}



	private:
		bool m_connected; 

		 MYSQL * m_mysql;
}; 

//typedef std::shared_ptr<DBConnection> DBConnectionPtr; 
typedef std::shared_ptr<DBConnection> DBConnectionPtr; 

