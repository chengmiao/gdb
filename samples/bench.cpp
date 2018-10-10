#include <sys/time.h>
#include <iostream> 
#include "gdb.hpp"

double cur_time()
{
     struct timeval  curtime; 
     gettimeofday(&curtime,NULL); 
     return curtime.tv_sec + curtime.tv_usec/1000000.0; 
}


using namespace gdp::db; 
int main() {

    GDb gdb("10.246.60.86",3306,"root","123456"); 
    gdb.init("ai_check"); 

    DBQuery query; 

    double beg = cur_time(); 
    query.select().from("rank_test_data");
    double end = cur_time(); 

    dlog("from %f  to %f , spend %f ",beg,end,end-beg); 

    beg = cur_time(); 
    gdb.get(query); 
    gdb.each(query, [&](ResultSetPtr res) {
            //std::cout << res->count() << std::endl;
            //dlog("rank type %d ,player id %lld",res->get_int32("rank_type") , res->get_int64("player_id")); 

            });
 
    end = cur_time(); 

    dlog("from %f  to %f , spend %f ",beg,end,end-beg); 

    return 0; 
}
