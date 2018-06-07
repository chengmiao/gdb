#pragma once 

#include "resultset.hpp"
#include <memory>

class DBConnection{

    public:
	
	DBConnection(const DBConnection&) = delete;  
	DBConnection& operator = (const DBConnection &) = delete; 

	DBConnection( ) { }
	int init(
		const std::string & db = "", 
		const std::string & user = "root",
		const std::string & passwd = "",
		const std::string& host  = "localhost", 
		int port = 3306 )

	{
	    m_db = db; 
	    m_host = host; 
	    m_user = user; 
	    m_passwd = passwd; 
	    m_port = port; 
	    mysql_init(&m_mysql);

	    MYSQL * res = mysql_real_connect(&m_mysql,
		    m_host.c_str(),
		    m_user.c_str(),
		    m_passwd.c_str(),
		    m_db.c_str(),
		    m_port,
		    NULL,
		    0);
	    if (res)
	    {
		my_bool reconnect = 1;
		mysql_options(&m_mysql, MYSQL_OPT_RECONNECT, &reconnect);
		m_connected = true; 
		std::cout << "connect to  mysql "<< m_user << ":" << m_passwd << "@"<< m_host <<":"<< m_port << " success" <<  std::endl; 
	    }
	    else 
	    {
		fprintf(stderr, "Failed to connect to database: Error: %s\n", mysql_error(&m_mysql));
	    }
	    return res != nullptr; 
	}

	bool use(const std::string &db)
	{
	    m_db = db; 
	    int ret = mysql_select_db(&m_mysql,db.c_str()); 
	    if (ret != 0)
	    {
		fprintf(stderr, "execute queue : Error: %s\n", mysql_error(&m_mysql));
	    }
	    return ret == 0 ; 
	}

	bool is_connected()
	{
	    return m_connected; 
	}

	//execute sql without result 
	bool execute(const std::string & sql ){

	    std::cout << "execute query :" << sql << std::endl; 
	    int ret = mysql_real_query(&m_mysql,sql.c_str(),sql.size() ); 
	    if (ret != 0 )
	    {
		fprintf(stderr, "execute queue : Error: %s\n", mysql_error(&m_mysql));
	    }
	    return ret  == 0 ; 
	}

	ResultSetPtr query(const std::string & sql) 
	{
	    int ret = mysql_real_query(&m_mysql,sql.c_str(),sql.size() ); 
	    if ( ret != 0) 
	    {
		fprintf(stderr, "execute queue : Error: %s\n", mysql_error(&m_mysql));
		return nullptr; 
	    }
	    MYSQL_RES *res = mysql_store_result(&m_mysql);
	    ResultSetPtr  rset  = std::make_shared<ResultSet>(res); 
	    return rset; 
	}

    private:
	bool m_connected = false; 
	std::string m_host = "localhost" ; 
	std::string m_db = ""; 
	std::string m_user  = "root" ; 
	std::string m_passwd = ""; 
	int m_port  = 3306; 
	MYSQL  m_mysql ;
}; 

typedef std::shared_ptr<DBConnection> DBConnectionPtr; 

