
#include "dbstmt.hpp"




int main()
{
    DBStmt dbStmt; 
    dbStmt.init( "ai_check","10.246.60.86","root","123456"); 


    dbStmt.prepare("select * from  rank_test_data where score > ? and score  < ? ",111,22222); 

    return 0; 
}
