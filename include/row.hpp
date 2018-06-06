#pragma once 



namespace gdp
{
    namespace db{

        class Row
		{
			public:

				static Row first(ResultSetPtr pRes)
				{
					Row row ; 
					row.row = pRst->first(); 
					return row; 
				}
			private:
				MYSQL_ROW	row  = nullptr;
		}; 


    }
}
