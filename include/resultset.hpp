#pragma once 
#include "mysql/mysql.h"
#include <unordered_map>
#include <string>
class ResultSet{


    public:
	ResultSet(MYSQL_RES * r ):m_res(r)
    {
	load_field_map(); 
    }
	~ResultSet()
	{
	    if (m_res)
	    {
		mysql_free_result(m_res); 
		m_res = nullptr; 
	    }
	}

	MYSQL_ROW first()
	{
	    mysql_field_seek(this->m_res, 0); 
	    return mysql_fetch_row(this->m_res); 
	}

	int  get_int(const std::string & key )
	{
	    if (m_row)
	    {
		auto field =  m_field_map.find(key); 
		if (field != m_field_map.end())
		{
		    unsigned int idx = field->second; 
		    return std::stoi(m_row[idx]); 
		}
	    }
	    return 0; 
	}
	std::string get_string(const std::string & key )
	{
	    if (m_row)
	    {
		auto field =  m_field_map.find(key); 
		if (field != m_field_map.end())
		{
		    unsigned int idx = field->second; 
		    return std::string(m_row[idx]); 
		}
	    }
	    return ""; 
	}

	bool next()
	{
	    if (m_res)
	    {
		m_row = mysql_fetch_row(m_res); 
		if (m_row)
		{
		    return true; 
		}
		return false; 
	    }
	    return false; 
	}

	my_ulonglong count()
	{
	    return mysql_num_rows(m_res); 
	}

    private:
	void load_field_map()
	{
	    unsigned int num  = mysql_num_fields(m_res);
	    MYSQL_FIELD * fields = mysql_fetch_fields(m_res); 
	    for(unsigned int i =0; i < num ; i ++)
	    {
		m_field_map[fields[i].name ]  = i ; 
	    }
	}

	MYSQL_RES* m_res  = nullptr;
	MYSQL_ROW  m_row  = nullptr; 
	std::unordered_map<std::string ,unsigned int > m_field_map; 

}; 	

typedef std::shared_ptr<ResultSet> ResultSetPtr; 
