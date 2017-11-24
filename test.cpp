/**
 * @file test.cpp
 * @brief 
 * @author arthur fatih@qq.com
 * @version 1.0.0
 * @date 2017-11-23
 */


#include <iostream>
#include "tinymysql.hpp"

int main()
{

    TinyMysql  myConn("127.0.0.1",3306,"root","Hello123"); 

    //std::cout << myConn.table("test").select ( "* ").where(" id ", "=", 1).where("name","=","test").order_by("id").get() << std::endl; 
    //std::cout << myConn.db("mysql").table("test").select ( "name", "sex","age" ).where(" id ", "=", 1).where("name","=","test").order_by("id").group_by("id").sql() ; 
    std::cout << myConn.db("mysql").table("test").select ( "name", "sex","age" ).where(" id ", "=", 1).where("name","=","test").order_by("id").sql() ; 

    myConn.get([](sql::ResultSet & ){


            }); 


    return 0; 
}
