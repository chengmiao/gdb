/**
 * @file googletest.cpp
 * @brief
 * @author arthur fatih@qq.com
 * @version 1.0.0
 * @date 2018-10-11
 */


#include <iostream>
#include <sys/time.h>
#include "gdb.hpp"
#include "gtest/gtest.h"

using namespace gdp::db;

double cur_time()
{
     struct timeval  curtime;
     gettimeofday(&curtime,NULL);
     return curtime.tv_sec + curtime.tv_usec/1000000.0;
}

class GDbTest : public ::testing::Test {
protected:
    virtual void SetUp(){
        testdb.add("10.246.60.86",3306,"root","123456");//"127.0.0.1",3306,"root","123456");
        testdb.init("ai_check");//"tinydb");

        testdb.execute(
            "create table if not exists user_info ("
            "uid            int             unsigned            not null,"
            "name           varchar(32)     character set utf8  not null,"
            "status         int                     ,"
            "score          float                      ,"
            "addr           varchar(32)                     ,"
            "primary key (uid)"
            ") engine=innodb default charset=utf8;"
            );

        testdb.execute(
            "create table if not exists addr_info ("
            "uid            int             unsigned            not null,"
            "km             int             ,"
            "addr           varchar(32)     not null        ,"
            "primary key (uid)"
            ") engine=innodb default charset=utf8;"
            );

    }
    virtual void TearDown(){

    }
    GDb testdb;
    DBQuery query;
};


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
//TEST_F(GDbTest, init){
//    testdb.add("10.246.60.86",3306,"root","3456");//"127.0.0.1",3306,"root","123456");
//    EXPECT_FALSE(testdb.init("ai_check").resultVal)<<testdb.init("ai_check").errorString;//"tinydb");
//
//    std::cout << testdb.init("ai_check").errorString << std::endl;
//    std::cout << "空格" << std::endl;
//    testdb.add("10.246.60.86",3306,"roo","123456");//"127.0.0.1",3306,"root","123456");
//    EXPECT_FALSE(testdb.init("ai_check").resultVal)<<testdb.init("ai_check").errorString;//"tinydb");
//
//    testdb.add("10.246.60.86",3306,"root","123456");//"127.0.0.1",3306,"root","123456");
//    EXPECT_TRUE(testdb.init("ai_check").resultVal)<<testdb.init("ai_check").errorString;//"tinydb");
//
//    EXPECT_TRUE(testdb.execute(
//        "create table if not exists user_info ("
//        "uid            int             unsigned            not null,"
//        "name           varchar(32)     character set utf8  not null,"
//        "status         tinyint         unsigned            not null,"
//        "score          float                      not null,"
//        "primary key (uid)"
//        ") engine=innodb default charset=utf8;"
//        ).resultVal)<<query.sql();
//
//    EXPECT_FALSE(testdb.execute(
//        "create table if not exists user_info ("
//        "uid            unsigned            not null,"
//        "name           varchar(32)     character set utf8  not null,"
//        "status         tinyint         unsigned            not null,"
//        "score          float                      not null,"
//        "primary key (uid)"
//        ") engine=innodb default charset=utf8;"
//        ).resultVal)<<query.sql();
//
//}

TEST_F(GDbTest, insert){
    for(int i = 0; i < 20; i ++) {
        std::stringstream  name;
        name << "test" << i ;

        std::stringstream  name1;
        name1 << "test" << i ;
        query.insert_or_update("user_info","uid", "name").values(i, name.str());
        EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();

    //    query.insert_into("app_info", "AppID", "Name").values(name.str(), name.str());
    //    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();

    //    query.insert_into("confirm_list", "ID", "Text", "Label", "AppID", "ServerID").values(i+2, name1.str(), 2, name.str(),"s1212");
    //    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();
    }
    for(int i = 10; i < 20; i ++) {
        std::stringstream  name;
        name << "test" << i+10 ;

        std::stringstream  name1;
        name1 << "test" << i ;
        query.insert_or_update("user_info","uid", "name","status", "score", "addr").values(i, name.str(),0, 0, "");
        EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();

    }
    for(int i = 20; i < 30; i ++) {
        std::stringstream  name;
        name << "test" << i+10 ;

        std::stringstream  name1;
        name1 << "test" << i ;
        query.insert_or_update("user_info","uid", "name","status", "score", "addr").values(i, name.str(),0, 2.234, "shanghai");
        EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();

    }
}

TEST_F(GDbTest, select_repeat){
    query.select().from("user_info").where("uid", "<", 5);
    auto result = testdb.get(query);
    EXPECT_TRUE(result.resultVal);
    EXPECT_TRUE(result.resultVal->next());
    auto res = result.resultVal->first();
    dlog("count: %llu, first: uid %d ,name %s, score %d",res->count(), res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("score"));

    std::string str = "select user_info.uid,sum(addr_info.km) from user_info,addr_info where user_info.uid = addr_info.uid and user_info.uid = 1";
    result = testdb.get(str);
    EXPECT_TRUE(result.resultVal);
    if(result.resultVal){
        EXPECT_FALSE(result.resultVal->next());
        res = result.resultVal->first();
        if(res)
            dlog("count: %llu, first: uid %d ,name %s, score %d",res->count(), res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("score"));
    }

    query.select().from("user_info").where("uid", "<", 0);
    result = testdb.get(query);
    EXPECT_TRUE(result.resultVal);
    EXPECT_FALSE(result.resultVal->next());
    res = result.resultVal->first();
    if(res)
        dlog("count: %llu, first3: uid %d ,name %s, score %d",res->count(), res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("score"));

    query.select().from("user_info").where("uid", "<", 5);
    result = testdb.first(query);
    EXPECT_TRUE(result.resultVal);
    EXPECT_TRUE(result.resultVal->next());
    res = result.resultVal;
    dlog("count: %llu, first: uid %d ,name %s, score %d",res->count(), res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("score"));
    res = res->first();
    dlog("first: uid %d ,name %s, score %d",res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("score"));

    query.select().from("user_info").where("uid", "<", 0);
    result = testdb.first(query);
    EXPECT_FALSE(result.resultVal);

}

TEST_F(GDbTest, update){
    for(int i = 5; i < 10; i ++) {
        std::stringstream  name;
        name << "test" << i ;
        query.update("user_info").set("status", 1).where("name",name.str());
        EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();
    }

    query.update("confirm_list").set("Label", 1).where("AppID","like", "test1%");
    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();

    query.update("confirm_list").set("ServerID", "s1215").where("AppID","like", "test1%").or_where("ID", "=", 0);
    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();

    query.update("confirm_list").set("Label", 4).set("ServerID", "s1213").where("AppID","like", "test1%").or_where("ID", "=", 0);
    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();
}

TEST_F(GDbTest, null_0){
    query.select().from("user_info").where("uid", "<", 5);
    EXPECT_TRUE(testdb.each(query,[&](ResultSetPtr res){

//                   elog("uid %d ,name %s, score %d",res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("score"));
           //        elog("status %d",res->get_int32("status"));
                }).resultVal)<<query.sql();

    EXPECT_TRUE(testdb.each(query,[&](ResultSetPtr res){

     //              elog("uid %d ,name %s, score %d",res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("score"));
//                   elog("status %d",res->get_int32("status"));
                }).resultVal)<<query.sql();

    query.select().from("user_info").where("uid", "<", 10).where("uid", ">", 4);
    EXPECT_TRUE(testdb.each(query,[&](ResultSetPtr res){

//                   elog("uid %d ,name %s, status %d, score %d",res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("status"), res->get_int32("score"));
                }).resultVal)<<query.sql();

    query.select().from("user_info").where("uid", ">", 9);
    EXPECT_TRUE(testdb.each(query,[&](ResultSetPtr res){

//                   elog("uid %d ,name %s, status %d, score %d",res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("status"), res->get_int32("score"));
                }).resultVal)<<query.sql();

    query.select().from("user_info").where("uid", "<", 10);
    EXPECT_TRUE(testdb.each(query,[&](ResultSetPtr res){

//                   elog("uid %d ,name %s, score %f",res->get_int32("uid"),res->get_string("name").c_str() , res->get_float("score"));
           //        elog("status %d",res->get_int32("status"));
                }).resultVal)<<query.sql();

    EXPECT_TRUE(testdb.each(query,[&](ResultSetPtr res){

     //              elog("uid %d ,name %s, score %d",res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("score"));
//                     elog("addr %s",res->get_string("addr").c_str());
                }).resultVal)<<query.sql();

    query.select().from("user_info").where("uid", "<", 20).where("uid", ">", 9);
    EXPECT_TRUE(testdb.each(query,[&](ResultSetPtr res){

//                   elog("uid %d ,name %s, status %d, score %f",res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("status"), res->get_float("score"));
//                   elog("addr %s",res->get_string("addr").c_str());
                }).resultVal)<<query.sql();

    query.select().from("user_info").where("uid", ">", 19);
    EXPECT_TRUE(testdb.each(query,[&](ResultSetPtr res){

//                   elog("uid %d ,name %s, status %d, score %f",res->get_int32("uid"),res->get_string("name").c_str() , res->get_int32("status"), res->get_float("score"));
//                   elog("addr %s",res->get_string("addr").c_str());
                }).resultVal)<<query.sql();
}

TEST_F(GDbTest, select_order_by_limit){

    query.select().from("rank_test_data").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQuery& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).order_by("player_id");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQuery& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).order_by("player_id", "asc");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

    // 此测试案例为错误query语句，执行结果为false,但使用了EXPECT_FALSE宏，因此测试结果为pass,后面有类似案例
    query.select().from("rank_test_data").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQuery& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).order_by("player_id", "des");
    EXPECT_FALSE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql()<<testdb.each(query, [&](ResultSetPtr res) { }).errorString;

    query.select().from("rank_test_data").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQuery& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,819036109);
    }).order_by("score");
    testdb.get(query, [&](ResultSetPtr res) {
        });

    query.select().from("rank_test_data").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQuery& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,819036109);
    }).order_by("score");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
         //   std::cout << res->count() << std::endl;
         //   elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }).resultVal)<<query.sql();

    query.select().from("confirm_list").where("Text","like","m%p%").where("Label", ">", 0).where([](DBQuery& query){
            query.where("ID" ,"<" ,100).or_where("ID" ,">" ,1);
    }).order_by("ServerID").limit(12);
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
  //          std::cout << res->count() << std::endl;
  //          elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s",
  //              res->get_int32("ID"), res->get_string("Text").str(), res->get_int32("Label"), res->get_string("AppID").str(), res->get_string("ServerID").str());

        }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQuery& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,819036109);
    }).order_by("score").limit(10, 40);
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
       //     std::cout << res->count() << std::endl;
       //     elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }).resultVal)<<query.sql();

    query.select().from("user_info").where("score","=",1.123).where("score",1.1234).where([](DBQuery& query){
            query.where("score" ,"<" ,1.25).or_where("score" ,">=" ,1.111);
    }).order_by("score").limit(10, 40);
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
       //     std::cout << res->count() << std::endl;
       //     elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }).resultVal)<<query.sql();

}

TEST_F(GDbTest, select_group){

    query.select().from("rank_test_data").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQuery& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).group_by("player_id");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","=","108114").where("player_id", "like", "8826906").where([](DBQuery& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    }).group_by("player_id").order_by("score");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
        }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQuery& query){
            query.where("score" ,"<" ,799040000).where("score" ,">=" ,719036109);
    }).group_by("player_id").order_by("score");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
        //    std::cout << res->count() << std::endl;
        //    elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","=","108114").where("player_id", ">", "8926906").where([](DBQuery& query){
            query.where("score" ,"<" ,799040000).where("score" ,">=" ,719036109);
    }).group_by("player_id").order_by("score").limit(5);
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
        //    std::cout << res->count() << std::endl;
        //    elog("rank type %d ,player id %ld, score %d",res->get_int32("rank_type") , res->get_int64("player_id"), res->get_int32("score"));

        }).resultVal)<<query.sql();

}

TEST_F(GDbTest, select_or_where){

    query.select().from("rank_test_data").where("rank_type","=","22011").where("player_id", "<", "8826906").where([](DBQuery& query){
            query.where("score" ,">" ,1000).where("score" ,"<=" ,719036109);
    });
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","r22011").where("player_id", "8826906").where([](DBQuery& query){
            query.where("player_id","234j342").or_where("score" ,"<=" ,719036109);
    }).order_by("player_id", "asc");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","like","1_8114%").where("player_id", "like", "%张三").where([](DBQuery& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,719036109);
    });
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","=","108114").where([](DBQuery& query){
            query.where("player_id", "<", "8826906").or_where("player_id" ,">=" ,"8h826906");
    });
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

// select  *  from confirm_list  where  (   ID < 10  and  Label >= 0   and   (  Text like "m%p%"  or  Text like "%p%"    )   )
    query.select().from("confirm_list").where([](DBQuery& query){
            query.where("ID" ,"<" ,10).where("Label" ,">=" ,0)
            .where([](DBQuery& query){
                    query.where("Text","like","m%p%").or_where("Text", "like", "%p%");
                });
            });
//    std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
//
//            std::cout << res->count() << std::endl;
//            elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
//                res->get_int32("ID"), res->get_string("Text").str(), res->get_int32("Label"), res->get_string("AppID").str(), res->get_string("ServerID").str(), res->get_string("Name").str());
                }).resultVal)<<query.sql();

// select  *  from confirm_list  where  (   ID < 10  and  Label >= 0    )  and   (  Text like "m%p%"  or  Text like "%p%"    )
    query.select().from("confirm_list").where([](DBQuery& query){
            query.where("ID" ,"<" ,10).where("Label" ,">=" ,0);
            }).where([](DBQuery& query){
                    query.where("Text","like","m%p%").or_where("Text", "like", "%p%");
                });

//    std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {

//            std::cout << res->count() << std::endl;
//            elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
//                res->get_int32("ID"), res->get_string("Text").str(), res->get_int32("Label"), res->get_string("AppID").str(), res->get_string("ServerID").str(), res->get_string("Name").str());
                }).resultVal)<<query.sql();

//select  *  from confirm_list  where  (   ID < 10  and  Label >= 0    ) and Text like "m%p%"
    query.select().from("confirm_list").where([](DBQuery& query){
            query.where("ID" ,"<" ,10).where("Label" ,">=" ,0);
    }).where("Text","like","m%p%");
//    std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {

//            std::cout << res->count() << std::endl;
//            elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
//                res->get_int32("ID"), res->get_string("Text").str(), res->get_int32("Label"), res->get_string("AppID").str(), res->get_string("ServerID").str(), res->get_string("Name").str());
                }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","=","108114").or_where("player_id", "<", "a8826906").where([](DBQuery& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,719036109);
    });
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

    query.select().from("rank_test_data").where("rank_type","=","108114").where([](DBQuery& query){
            query.where("player_id" ,"<" ,8826906).or_where("score" ,">=" ,8826950);

        }).where([](DBQuery& query){
            query.where("score" ,"<" ,1000).or_where("score" ,">=" ,719036109);
    });
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) { }).resultVal)<<query.sql();

}

TEST_F(GDbTest, select_inner_join){

    query.select().from("app_info").join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
        }).resultVal)<<query.sql();

    // 下面两个案例是错误输入 EXPECT_FALSE
    query.select().from("app_info").join("confirm_list", "app_info.App", "=", "confirm_list.AppID");
    EXPECT_FALSE(testdb.each(query, [&](ResultSetPtr res) {
        }).resultVal)<<query.sql();

    query.select().from("app_info").join("confirm_list", "AppID", "=", "AppID");
    EXPECT_FALSE(testdb.each(query, [&](ResultSetPtr res) {
        }).resultVal)<<query.sql();

    query.select().from("app_info").inner_join("confirm_list", "app_info.AppID", ">", "confirm_list.AppID");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
        }).resultVal)<<query.sql();

    query.select().from("app_info").inner_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
        }).resultVal)<<query.sql();

    query.update("confirm_list").inner_join("app_info", "app_info.AppID", "=", "confirm_list.AppID")
        .set("app_info.PrivateKey","127").where("confirm_list.Label", 4);
  //  std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();

    query.update("confirm_list a").inner_join("app_info b", "a.AppID", "=", "b.AppID")
        .set("b.PrivateKey","129").where("a.Label", 3);
  //  std::cout << query.sql() << std::endl;
    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();

    query.select().from("app_info").inner_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID")
        .where("ServerID", ">", "s12121").or_where("app_info.AppID", ">", "1212")
        .group_by("confirm_list.ID").limit(10);
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
//            std::cout << res->count() << std::endl;
//            elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
//                res->get_int32("ID"), res->get_string("Text").str(), res->get_int32("Label"), res->get_string("AppID").str(), res->get_string("ServerID").str(), res->get_string("Name").str());

        }).resultVal)<<query.sql();

    query.select().from("app_info").inner_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID")
        .where("ID", ">", 1)
        .where([](DBQuery& query){
            query.where("ServerID", ">", "s12121").or_where("ServerID", "=", "12121");
                })
        .where("Label", "=", 2);
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
    //      std::cout << res->count() << std::endl;
    //      elog("ID %d ,Text %s, Label %d, AppID %s, ServerID %s, Name %s",
    //          res->get_int32("ID"), res->get_string("Text").str(), res->get_int32("Label"), res->get_string("AppID").str(), res->get_string("ServerID").str(), res->get_string("Name").str());

        }).resultVal)<<query.sql();
}

TEST_F(GDbTest, select_left_join){

    query.select().from("app_info").left_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
        }).resultVal)<<query.sql();

}

TEST_F(GDbTest, select_right_join){

    query.select().from("app_info").right_join("confirm_list", "app_info.AppID", "=", "confirm_list.AppID");
    EXPECT_TRUE(testdb.each(query, [&](ResultSetPtr res) {
        }).resultVal)<<query.sql();

}

//TEST_F(GDbTest, del){
//
//    query.del("user_info").where("uid",1).or_where("score","=",1.123);
//    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();
//
//    query.del("user_info").where("uid",2).where("name","=","test1");
//    EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();
//
//    for(int i = 0; i < 30; i ++) {
//        std::stringstream  name;
//        name << "test" << i ;
//        query.del("user_info").where("uid", i);
//        EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();
//
////        query.del("app_info").where("AppID", name.str());
////        EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();
//
////        query.del("confirm_list").where("AppID", name.str());
////        EXPECT_TRUE(testdb.execute(query).resultVal)<<query.sql();
//    }
//}
