/**
 * @file test.cpp
 * @brief 
 * @author arthur fatih@qq.com
 * @version 1.0.0
 * @date 2017-11-23
 */


#include <iostream>
#include "gdb.hpp"

using namespace gdp::db; 
int main() {

    GDb testdb("127.0.0.1",3306,"root","Hello123"); 
    testdb.init("gdb"); 

    DBQuery query; 

    testdb.execute(
        "create table if not exists user_info ("
            "uid            int             unsigned            not null,"
            "name           varchar(32)     character set utf8  not null,"
            "status         tinyint         unsigned            not null,"
            "primary key (uid)"
        ") engine=innodb default charset=utf8;"
    );

    for(int i = 0; i < 10; i ++) {
        fmt::MemoryWriter name; 
        name << "test" << i ; 
        query.table("user_info").insert("uid", "name","status").values(i, name.c_str(), 0); 
        testdb.execute(query); 
    }

    query.table("user_info").select("uid", "name");
    testdb.get(query, [](ResultSetPtr res) {
                int row = 0;
		std::cout << "total : " << res->count() << std::endl; 
                while(res->next()) {
                    //std::cout << row << ", " << res.getRow() << std::endl;
                    std::cout << "uid=" << res->get_int("uid");
                    std::cout << ", name=" << res->get_string("name") << std::endl;
                    row++;
                }
            });

    //testdb.execute(query); 
    bool has_user = false;
    query.table("user_info").select("uid", "name");
    testdb.get(query, [&has_user](ResultSetPtr res) {
                std::cout << res->count() << std::endl;
                has_user = res->count() > 0;
    });
    std::cout << "has_user: " << has_user << std::endl;

    for (int i = 0; i < 10; i++) {
        fmt::MemoryWriter name;
        name << "test" << i;
        query.table("user_info").update().set("status", 1);
        testdb.execute(query);
    }

    for (int i = 0; i < 10; i++) {
        fmt::MemoryWriter name;
        name << "test" << i;
        query.table("user_info").del().where("name", name.c_str());
        testdb.execute(query);
    }
    
    query.table("user_info").select("uid", "name");
    testdb.get(query, [&has_user](ResultSetPtr res) {
                std::cout << res->count() << std::endl;
                has_user = res->count() > 0;
    });
    std::cout << "has_user: " << has_user << std::endl;
    
    return 0; 
}
