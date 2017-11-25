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

    TinyMysql  myConn("127.0.0.1",3306,"root","Hello123","tinydb"); 

    //std::cout << myConn.table("test").select ( "* ").where(" id ", "=", 1).where("name","=","test").order_by("id").get() << std::endl; 
    //std::cout << myConn.db("mysql").table("test").select ( "name", "sex","age" ).where(" id ", "=", 1).where("name","=","test").order_by("id").group_by("id").sql() ; 
//    std::cout << myConn.db("mysql").table("test").select ( "name", "sex","age" ).where(" id ", "=", 1).where("name","=","test").order_by("id").sql() ; 

 //   myConn.get([](sql::ResultSet & ){


 //           }); 

    for(int i = 0; i < 10; i ++ )
    {
	fmt::MemoryWriter name ; 
	name << "test" << i ; 
	TinyMysql & queue = myConn.table("users").insert("name", "age","sex","phone","address").values( name.c_str() ,20+i,1,"18930878762","shanghai"); 
	//std::cout << queue.sql() << std::endl; 

	queue.execute(); 
    }

    //std::cout << myConn.table("users").update().set("phone","8888888").where("name","arthur").sql(); 
    TinyMysql& queue = myConn.table("users").update().set("phone","8888888").where("name","arthur"); 

    queue.execute(); 



    for(int i = 3;i < 8; i ++ )
    {

	fmt::MemoryWriter name ; 
	name << "test" << i ; 
	queue.del().where("name" ,name.c_str()); 
	queue.execute(); 
    }


    return 0; 
}
