#pragma once 

#include "resultset.hpp"

namespace gdp
{
    namespace db{

	class Row
	{
	    public:

		static Row first(ResultSetPtr pRes)
		{
		    Row row ; 
		    row.row = pRes->first(); 
		    return row; 
		}
		    

	    private:
		MYSQL_ROW	row  = nullptr;
	}; 


    }
}
