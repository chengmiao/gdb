#pragma once 

#include "resultset.hpp"
#include <memory>

class DBConnection{

    public:

	DBConnection(
		const std::string & db  = "", 
		const std::string & user = "root",
		const std::string & passwd = "",
		const std::string& host  = "localhsot", 
		int port = 3306 )
	{
	    m_host = host; 
	    m_user = user; 
	    m_passwd = passwd; 

	    m_db = db; 
	    m_port = port; 
	}

	int init()
	{
	    mysql_init(m_mysql);
	    my_bool reconnect = 1;
	    mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);

	    MYSQL * res = mysql_real_connect(m_mysql,
		    m_host.c_str(),
		    m_user.c_str(),
		    m_passwd.c_str(),
		    m_db.c_str(),
		    m_port,
		    NULL,
		    0);
	    return res != nullptr; 


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
	    ResultSetPtr  rset  = std::make_shared<ResultSet>(res); 
	    return rset; 
	}
	int use_db(const std::string &db)
	{
	    return mysql_select_db(m_mysql,db.c_str()); 
	}



    private:
	bool m_connected; 

	std::string m_host = "localhost" ; 
	std::string m_db = ""; 
	std::string m_user  = "root" ; 
	std::string m_passwd = ""; 
	int m_port  = 3306; 
	MYSQL * m_mysql;
}; 

//typedef std::shared_ptr<DBConnection> DBConnectionPtr; 
typedef std::shared_ptr<DBConnection> DBConnectionPtr; 

