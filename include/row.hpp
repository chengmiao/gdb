#pragma once 
#include <unordered_map>
#include <string>
#include "mysql/mysql.h"

namespace gdp
{
    namespace db{

	class Row
	{
	    public:
		typedef std::unordered_map<std::string ,unsigned int > FieldMap; 

		Row( const MYSQL_ROW & r, FieldMap * fMap =nullptr):m_row(r),m_field_map(fMap)
	    {
	    }
		~Row()
		{
		    m_row = nullptr; 
		}

		int  get_int(const std::string & key )
		{
		    if (m_row)
		    {
			if (m_field_map)
			{
			    auto field =  m_field_map->find(key); 
			    if (field != m_field_map->end())
			    {
				unsigned int idx = field->second; 
				return std::stoi(m_row[idx]); 
			    }
			}
		    }
		    return 0; 
		}

		std::string get_string(const std::string & key )
		{
		    if (m_row)
		    {
			if (m_field_map)
			{
			    auto field =  m_field_map->find(key); 
			    if (field != m_field_map->end())
			    {
				unsigned int idx = field->second; 
				return std::string(m_row[idx]); 
			    }
			}
		    }
		    return ""; 
		}
		float get_float(const std::string & key)
		{
		    if (m_row)
		    {
			if (m_field_map)
			{

			    auto field =  m_field_map->find(key); 
			    if (field != m_field_map->end())
			    {
				unsigned int idx = field->second; 
				return std::stof(m_row[idx]); 
			    }
			}

		    }
		    return 0; 
		}

	    private:
		MYSQL_ROW m_row  = nullptr;
		FieldMap * m_field_map = nullptr; 
	}; 


    }
}
