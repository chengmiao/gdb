#pragma once 
#include "mysql/mysql.h"
#include <vector>
#include <string>
#include "logger.hpp"
/*
typedef buffer_type int; 
template <typename T>
    struct ParamType{

        buffer_type operator ()(T & t)
        {
            return  MYSQL_TYPE_NULL; 
        }
    }; 
template <>
struct ParamType<std::string>{
        buffer_type operator ()(T & t)
        {
            return MYSQL_TYPE_STRING; 
        }
}; 
template <>
struct ParamType<int>{
        buffer_type operator ()(T & t)
        {
            return MYSQL_TYPE_LONG; 
        }
}; 
template <>
struct ParamType<long long int>{
        buffer_type operator ()(T & t)
        {
            return MYSQL_TYPE_LONGLONG; 
        }
}; 
template <>
struct ParamType<float>{
        buffer_type operator ()(T & t)
        {
            return MYSQL_TYPE_FLOAT; 
        }
}; 
template <>
struct ParamType<double>{
        buffer_type operator ()(T & t)
        {
            return MYSQL_TYPE_DOUBLE; 
        }
}; 
*/
typedef std::vector<MYSQL_BIND> Binds; 


template<typename First, typename... Rest>
struct BuildBinder
{
    Binds & m_binders; 
    BuildBinder(Binds & bds):m_binders(bds){ }
    void proc(First elem,Rest ...rest)
    {
        BuildBinder<Rest...> procor(m_binders) ; 
        procor.proc(rest...); 
    }
};

template<typename Last>
struct BuildBinder<Last>
{

    Binds & m_binders; 
    BuildBinder(Binds & bds):m_binders(bds){ }
    void proc(Last last)
    {
        dlog("finished bind"); 
    }
};





class DBStmt{


    public:
        void init()
        {
            m_mysql = mysql_init(nullptr);
            mysql_thread_init();
        }


        template<typename ... Args>
            bool prepare(const std::string & sql, const Args & ... args ) {
                MYSQL_STMT *stmt  = mysql_stmt_init(m_mysql);

                int argLen = sizeof ...(Args); 
                Binds binds; 
                if(mysql_stmt_prepare(stmt,sql.c_str(),sql.length()) )
                {
                    elog("mysql_stmt_prepare : %s failed",sql.c_str()); 
                }
                int param_count = mysql_stmt_param_count(stmt);
                binds.reserve(param_count); 
                BuildBinder<Args...> binder(binds); 
                binder.proc(args...);

            }

    private:

        MYSQL *m_mysql;
        MYSQL_STMT    *stmt;
} ; 
