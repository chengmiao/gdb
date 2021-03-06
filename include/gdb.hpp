#pragma once

/*
 * Feature List:
 * 1. db connection pool
 *
 *
 */

#include <vector>
#include <memory>
#include "mysql.h"
#include <iostream>
#include <map>
#include "fmt/format.h"
#include <functional>
//#include "sqlutils.h"
#include "dbquery.hpp"
#include "dbconnection.hpp"

namespace gdp
{
    namespace db
    {
#define MAX_CONN_COUNT 8
        struct DBConfig
        {
            DBConfig(const std::string & h = "127.0.0.1",
                    int pt = 3306,
                    const std::string & u = "root",
                    const std::string & p = "",
                    const std::string & n = "default")
            {
                name = n;
                host = h;
                port =pt;

                user = u;
                pass = p;

                pool_size = 1;
                connection = nullptr;
            }

            std::string name;
            std::string host;
            int port;
            std::string user;
            std::string pass;
            DBConnectionPtr   connection;
            int pool_size;
        };

        class GDb
        {
            public:
                typedef std::function<void(ResultSetPtr ) > ResultHandler;

                GDb(const std::string & host ="127.0.0.1",
                        int port = 3316,
                        const std::string & user = "root",
                        const std::string & passwd = "", const std::string& db = "",const std::string &name="default")
                {
                    m_configs[name] =  DBConfig(host,port,user,passwd,name);
                }
                void add(
                        const std::string & host ="127.0.0.1",
                        int port = 3306,
                        const std::string & user = "root",
                        const std::string & passwd = "",
                        const std::string & name= "default"
                        )
                {
                    m_configs[name] =  DBConfig(host,port,user,passwd,name);
                }

                SqlResult<bool> init(const std::string & db){
                    m_db = db;
                    return connect();

                }

                SqlResult<bool> connect() {
                    SqlResult<bool> retRes;
                    retRes.resultVal = false;

                    for(auto && cfg:m_configs)
                    {
                        DBConfig&  dbInfo = cfg.second;

                        DBConnectionPtr conn = std::make_shared<DBConnection>();
                        retRes = conn->init(m_db,dbInfo.user, dbInfo.pass,dbInfo.host,dbInfo.port);
                        if (conn->is_connected())
                        {
                            dbInfo.connection = conn;
                            if (dbInfo.name == "default" )
                            {
                                m_default = cfg.second;
                            }
                            dlog("connect to db success");
                            retRes.resultVal = true;
                            return retRes;
                        }
                        else
                        {
                            elog("connect to db failed");
                            retRes.resultVal = false;
                            retRes.errorString = "connect to db failed";
                            return retRes;

                        }
                    }

                    retRes.resultVal = false;
                    retRes.errorString = "cfg:m_configs error";
                    return retRes;
                }

                GDb & usedb(const std::string & dbName)
                {
                    m_default.connection->use(dbName);
                    return *this;
                }

                SqlResult<ResultSetPtr> get(const std::string & sql){
                    SqlResult<ResultSetPtr> retRes;
                    retRes.resultVal = nullptr;

                    if (!is_valid()) {
                        connect();
                    }
                    if (is_valid())
                    {
                        return m_default.connection->query(sql);
                    }
                    retRes.resultVal = nullptr;
                    retRes.errorString = "failed to connect to database";
                    return retRes;
                }

                SqlResult<ResultSetPtr> get(DBQuery & query){
                    SqlResult<ResultSetPtr> retRes;
                    retRes.resultVal = nullptr;

                    if (!is_valid()) {
                        connect();
                    }
                    if (is_valid())
                    {
                       // elog("get return connectiion->query");
                        return m_default.connection->query(query.sql());
                    }
                    //elog("get return nullptr");
                    retRes.resultVal = nullptr;
                    retRes.errorString = "failed to connect to database";
                    return retRes;
                }

                bool is_valid()
                {
                    if (m_default.connection )
                    {
                        return m_default.connection->is_connected();
                    }
                    return false;
                }

                void get(DBQuery& query , ResultHandler func)
                {
                    if (!is_valid()) {
                        connect();
                    }
                    if (is_valid())
                    {
                        auto result = m_default.connection->query(query.sql());
                        func(result.resultVal);
                    }
                }

                void get( ResultHandler func )
                {
                    return get(m_query,func);
                }

                SqlResult<bool> each(DBQuery& query , ResultHandler func)
                {
                    SqlResult<bool> retRes;
                    retRes.resultVal = false;
                    dlog("execute sql : %s",query.sql().c_str());

                    if (!is_valid()) {
                        connect();
                    }
                    if (is_valid())
                    {
                        auto result = m_default.connection->query(query.sql());
                        if(result.resultVal)
                        {
                            while(result.resultVal->next())
                            {
                                func(result.resultVal);
                            }
                            retRes.resultVal = true;
                            return retRes;
                        }

                        retRes.resultVal = false;
                        retRes.errorString = result.errorString;
                        return retRes;
                    }

                    retRes.resultVal = false;
                    retRes.errorString = "failed to connect to database";
                    return retRes;
                }

                SqlResult<ResultSetPtr> first(DBQuery & query ){
                    SqlResult<ResultSetPtr> retRes;
                    retRes.resultVal = nullptr;

                    if (!is_valid()) {
                        connect();
                    }

                    if (is_valid())
                    {
                        auto res = m_default.connection->query(query.sql());
                        retRes.resultVal = res.resultVal->first();
                        return retRes;
                    }

                    retRes.resultVal = nullptr;
                    retRes.errorString = "failed to connect to database";
                    return retRes;
                }

                SqlResult<ResultSetPtr> first(){
                    return first(m_query);
                }

                template<typename ... Args>
                    SqlResult<bool> execute(const char* format, const Args & ... args ) {
                        SqlResult<bool> retRes;
                        retRes.resultVal = false;
                        std::string sql = fmt::format( format, args... );
                        if (!is_valid()) {
                            connect();
                        }
                        if (is_valid())
                        {
                            dlog("execute sql :%s",sql.c_str());
                            return m_default.connection->execute(sql);
                        }

                        retRes.resultVal = false;
                        retRes.errorString = "failed to connect to database";
                        return retRes;
                    }

                SqlResult<bool> execute(const DBQuery & query)
                {
                    SqlResult<bool> retRes;
                    retRes.resultVal = false;
                    if (!is_valid()) {
                        connect();
                    }
                    if (is_valid())
                    {
                        return m_default.connection->execute(query.sql());
                    }
                    retRes.resultVal = false;
                    retRes.errorString = "failed to connect to database";
                    return retRes;
                }

                SqlResult<bool> execute(const std::string & sql)
                {
                    SqlResult<bool> retRes;
                    retRes.resultVal = false;
                    if (!is_valid()) {
                        connect();
                    }
                    if (is_valid())
                    {
                        return m_default.connection->execute(sql);
                    }
                    retRes.resultVal = false;
                    retRes.errorString = "failed to connect to database";
                    return retRes;
                }

                my_ulonglong get_last_insert_id()
                {
                    return m_default.connection->get_insert_id();
                }

                SqlResult<bool> execute()
                {
                    return execute(m_query);
                }

                int get_affected_rows()
                {
                    return m_default.connection->get_affected_rows();
                }

                DBQuery & query()
                {
                    return m_query;
                }

            private:
                std::map<std::string ,DBConfig> m_configs;
                DBQuery m_query ;
                std::string m_db;
                DBConfig m_default;
        };
    }
}
