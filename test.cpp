/**
 * @file test.cpp
 * @brief 
 * @author arthur fatih@qq.com
 * @version 1.0.0
 * @date 2017-11-23
 */


#include <iostream>
#include "tinydb.hpp"

int main()
{

    TinyDB  myConn("127.0.0.1",3306,"root","Hello123"); 

	DBQueue queue; 
	//std::cout << tinydb.table("test").select ( "* ").where(" id ", "=", 1).where("name","=","test").order_by("id").get() << std::endl; 
	//std::cout << tinydb.db("mysql").table("test").select ( "name", "sex","age" ).where(" id ", "=", 1).where("name","=","test").order_by("id").group_by("id").sql() ; 
	//    std::cout << tinydb.db("mysql").table("test").select ( "name", "sex","age" ).where(" id ", "=", 1).where("name","=","test").order_by("id").sql() ; 

	//   tinydb.get([](sql::ResultSet & ){


	//           }); 

	for(int i = 0; i < 10; i ++ )
	{
		fmt::MemoryWriter name ; 
		name << "test" << i ; 
		queue.table("users").insert("name", "age","sex","phone","address").values( name.c_str() ,20+i,1,"18930878762","shanghai"); 
		//std::cout << queue.sql() << std::endl; 
		tinydb.execute(queue); 

	}

	//std::cout << tinydb.table("users").update().set("phone","8888888").where("name","arthur").sql(); 
	queue.table("users").update().set("phone","8888888").where("name","arthur"); 

	tinydb.execute(queue); 



	for(int i = 3;i < 8; i ++ )
	{

		fmt::MemoryWriter name ; 
		name << "test" << i ; 
		queue.del().where("name" ,name.c_str()); 
		tinydb.execute(queue); 
	}


	return 0; 
}
