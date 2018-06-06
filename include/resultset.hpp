#pragma once 
#include "mysql/mysql.h"

class ResultSet{

	
	public:
		ResultSet(MYSQL_RES * r ):res(r)
		{
		}
		~ResultSet()
		{
			if (res)
			{
				mysql_free_result(res); 
				res = nullptr; 
			}

		}

		MYSQL_ROW first()
		{
			mysql_field_seek(this->res, 0); 
			return mysql_fetch_row(this->res); 
		}


		MYSQL_RES* 			res  = nullptr;

}; 	

typedef std::shared_ptr<ResultSet> ResultSetPtr; 
