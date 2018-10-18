#pragma once
#include "mysql.h"
#include <vector>
#include <string.h>
#include "logger.hpp"

template <typename T>
    struct ParamType{
        enum_field_types  get_type(T  t)
        {
            return  MYSQL_TYPE_NULL;
        }
        unsigned long get_length(T t )
        {
            return 0 ;
        }
    };

template <>
struct ParamType<char * >{
        enum_field_types  get_type(const char *   t)
        {
            return MYSQL_TYPE_STRING;
        }
        unsigned long get_length(const char *  t)
        {
            return strlen(t);
        }
};
template <>
struct ParamType<std::string>{
        enum_field_types  get_type(const std::string &    t)
        {
            return MYSQL_TYPE_STRING;
        }
        unsigned long get_length(const std::string & t)
        {
            return t.length();
        }
};
template <>
struct ParamType<int>{
        enum_field_types  get_type(int   t)
        {
            return MYSQL_TYPE_LONG;
        }
        unsigned long get_length(int t ) { return 0; }

};
template <>
struct ParamType<long long int>{
        enum_field_types  get_type(long long int )
        {
            return MYSQL_TYPE_LONGLONG;
        }
        unsigned long get_length(long long int t ) { return 0; }
};
template <>
struct ParamType<float>{
        enum_field_types  get_type(float)
        {
            return MYSQL_TYPE_FLOAT;
        }
        unsigned long get_length(float t ) { return 0; }
};
template <>
struct ParamType<double>{
        enum_field_types  get_type(double  t)
        {
            return MYSQL_TYPE_DOUBLE;
        }
        unsigned long get_length(double t ) { return 0; }
};

typedef std::vector<MYSQL_BIND> Binds;


template<typename First, typename... Rest>
struct BuildBinder
{
    Binds & m_binders;
    BuildBinder(Binds & bds):m_binders(bds){ }
    void proc(const First & elem,Rest ...rest)
    {
        BuildBinder<Rest...> procor(m_binders) ;

        MYSQL_BIND  bind;
        memset(&bind, 0, sizeof(bind));
        ParamType<First> pType;
        bind.buffer_type = pType.get_type(elem) ;
        bind.buffer = (char *) &elem;
        unsigned long len = pType.get_length(elem);
        bind.length = &len;
        bind.is_null = 0;
        m_binders.push_back(bind);
        procor.proc(rest...);
    }
};

template<typename Last>
struct BuildBinder<Last>
{
    Binds & m_binders;
    BuildBinder(Binds & bds):m_binders(bds){ }
    void proc(const Last & elem)
    {
        MYSQL_BIND  bind;
        memset(&bind, 0, sizeof(bind));
        ParamType<Last> pType;
        bind.buffer_type = pType.get_type(elem) ;
        bind.buffer = (char *) &elem;
        unsigned long len = pType.get_length(elem);
        bind.length = &len;
        bind.is_null = 0;
        m_binders.push_back(bind);

        dlog("finished bind");
    }
};



class DBStmt{
    public:
        void init(const std::string &db ,const std::string & host = "localhost",const std::string & user = "root",const std::string &passwd  = "",int port  = 3306 )
        {
            m_mysql = mysql_init(nullptr);
            mysql_thread_init();
            MYSQL *res = mysql_real_connect(m_mysql, host.c_str(), user.c_str(),
                                    passwd.c_str(), db.c_str(), port, NULL, 0);

        }

        template<typename ... Args>
            bool prepare(const std::string & sql, const Args & ... args ) {
                MYSQL_STMT *stmt  = mysql_stmt_init(m_mysql);

                int argLen = sizeof ...(Args);
                Binds binds;

                int status = mysql_stmt_prepare(stmt,sql.c_str(),sql.length()) ;
                if (status !=0 )
                {
                    elog("mysql_stmt_prepare : %s failed, error %s ",sql.c_str(),mysql_error(m_mysql));
                }
                int param_count = mysql_stmt_param_count(stmt);
                binds.reserve(param_count);
                BuildBinder<Args...> binder(binds);
                binder.proc(args...);

                status = mysql_stmt_bind_param(stmt, binds.data());
                if (status !=0 )
                {
                    elog("mysql_stmt_bind_param: %s failed, error %s ",sql.c_str(),mysql_error(m_mysql));
                }

                status = mysql_stmt_execute(stmt);
                if (status !=0 )
                {
                    elog("mysql_stmt_execute: %s failed, error %s ",sql.c_str(),mysql_error(m_mysql));
                }



                //get result


                int        int_data[3];   /* input/output values */
                my_bool    is_null[3];    /* output value nullability */
                int num_fields = mysql_stmt_field_count(stmt);

                MYSQL_RES *rs_metadata = mysql_stmt_result_metadata(stmt);
                if (rs_metadata == NULL)
                {
                    elog("no meta data");
                }

                MYSQL_FIELD * fields = mysql_fetch_fields(rs_metadata);
                MYSQL_BIND * rs_bind = (MYSQL_BIND *) malloc(sizeof (MYSQL_BIND) * num_fields);
                if (!rs_bind)
                {
                    printf("Cannot allocate output buffers\n");
                    exit(1);
                }

                memset(rs_bind, 0, sizeof (MYSQL_BIND) * num_fields);

                /* set up and bind result set output buffers */
                for (int i = 0; i < num_fields; ++i)
                {
                    rs_bind[i].buffer_type = fields[i].type;
                    rs_bind[i].is_null = &is_null[i];

                    switch (fields[i].type)
                    {
                        case MYSQL_TYPE_LONG:
                            rs_bind[i].buffer = (char *) &(int_data[i]);
                            rs_bind[i].buffer_length = sizeof (int_data);
                            break;
                        case MYSQL_TYPE_STRING:
                            rs_bind[i].buffer = (char *) malloc(256);
                            rs_bind[i].buffer_length = 256;
                            break;
                        case MYSQL_TYPE_FLOAT:
                            rs_bind[i].buffer = (char *) malloc(256);
                            rs_bind[i].buffer_length = sizeof(float);
                            break;
                        case MYSQL_TYPE_LONGLONG:

                            dlog("mysql type is longlong");
                            rs_bind[i].buffer = (char *) malloc(sizeof(long long int) );
                            rs_bind[i].buffer_length = sizeof(long long int);
                            break;
                        case MYSQL_TYPE_DOUBLE:
                            dlog("mysql type is double");

                            rs_bind[i].buffer = (char *) malloc(sizeof(double) );
                            rs_bind[i].buffer_length = sizeof(double);
                            break;
                        case MYSQL_TYPE_TINY:
                            dlog("mysql type is tiny");

                            rs_bind[i].buffer = (char *) malloc(sizeof(int) );
                            rs_bind[i].buffer_length = sizeof(int);
                            break;


                        default:
                            fprintf(stderr, "ERROR: unexpected type: %d.\n", fields[i].type);
                            exit(1);
                    }
                }

                dlog("get file count %d",num_fields);
                status = mysql_stmt_bind_result(stmt, rs_bind);
                while(status == 0)
                {
                    status = mysql_stmt_fetch(stmt);

                    if (status == 1 || status == MYSQL_NO_DATA)
                        break;

                    for (int i = 0; i < num_fields; ++i)
                    {
                        switch (rs_bind[i].buffer_type)
                        {
                            case MYSQL_TYPE_LONG:
                                if (*rs_bind[i].is_null)
                                    printf(" val[%d] = NULL;", i);
                                else
                                    printf(" val[%d] = %ld;",
                                            i, (long) *((int *) rs_bind[i].buffer));
                                break;
                            case MYSQL_TYPE_STRING:
                                dlog("mysql type is string ");
                                break;
                        case MYSQL_TYPE_LONGLONG:

                                dlog("mysql type is string ");
                                break;

                            default:
                                printf("  unexpected type (%d)\n", rs_bind[i].buffer_type);
                        }
                    }


                    status = mysql_stmt_next_result(stmt);
                }

                return true;
            }

    private:
        MYSQL *m_mysql;
        MYSQL_STMT    *stmt;
} ;
