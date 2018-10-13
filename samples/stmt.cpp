
#include "dbstmt.hpp"




int main()
{
    DBStmt dbStmt; 
    dbStmt.init(); 


    dbStmt.prepare("select * from user_infos where id= ? and id  < ? ",111,222); 

    return 0; 
}
