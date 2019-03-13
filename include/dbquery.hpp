#pragma once
#include "escape_string.h"
#include "resultset.hpp"
#include "logger.hpp"
#include <stack>
#include <sstream>

#include "sol.hpp"

namespace gdp
{
    namespace db
    {
        class DBQuery
        {
            public:
                static const uint32_t MAX_NEST_LEVEL = 16;
                typedef std::function<void(DBQuery & ) > SelfHandler;
                DBQuery(const std::string & tbName = "")
                {
                    m_table = tbName;
                }

                DBQuery & create(const std::string &tbName,bool check = false)
                {
                    if (check) {
                        fmt::format_to(m_sql," {} IF NOT EXISTS " , tbName);
                    }
                    else {
                        fmt::format_to(m_sql," {} " , tbName);
                    }
                    return *this;
                }

                DBQuery & begin()
                {
                    fmt::format_to(m_sql, " ( ") ;
                    return *this;
                }

                DBQuery & end()
                {
                    fmt::format_to(m_sql, " ) ") ;
                    return *this;
                }

                DBQuery & def(const std::string  & key ,
                        const std::string & type,
                        int length = 0 ,
                        const std::string & dft = "",bool inc = false){

                    if (definitions.size() > 0)
                    {
                        fmt::format_to(m_sql, " , ") ;
                    }

                    if (length >0)
                    {
                        std::string defStr = fmt::format(" {} {}({}) {} ", key,type,length, inc ? " AUTO_INCREMENT " :"" );
                        fmt::format_to(m_sql,defStr);
                    }
                    else {
                        std::string defStr = fmt::format(" {} {} {} ", key,type, inc ? " AUTO_INCREMENT " :"" );
                        fmt::format_to(m_sql,defStr);
                    }
                    return *this;
                }

                template <typename ... Args>
                    DBQuery & insert_into(const std::string & tbName ,const Args & ... args)
                    {
                        clear();
                        int argLen = sizeof ...(Args);
                        m_table = tbName;
                        fmt::format_to(m_sql,"insert into {} " ,tbName);
                        if (argLen > 0 )
                        {
                            fmt::format_to(m_sql, " ( ") ;
                        }
                        std::stringstream format;
                        for (int i  = 0; i < argLen ; i++)
                        {
                            if (i < argLen -1 ) {
                                format<< " {}, ";
                            }
                            else {
                                format<< " {} ";
                            }
                        }
                        fmt::format_to(m_sql,format.str(),args...);

                        if (argLen > 0)
                        {
                            fmt::format_to(m_sql," ) " );
                        }
                        return *this;
                    }

                    DBQuery & to_lua_insert_into(const std::string & tbName, sol::variadic_args args)
                    {
                        clear();
                        int argLen = args.size();
                        m_table = tbName;
                        fmt::format_to(m_sql,"insert into {} " ,tbName);
                        if (argLen > 0 )
                        {
                            fmt::format_to(m_sql, " ( ") ;
                        }
                        std::stringstream format;
                        int i = 0;
                        for ( auto v : args)
                        {
                            if (i < argLen -1 ) {
                                format<< " {}, ";
                            }
                            else {
                                format<< " {} ";
                            }

                            ++i;
                            fmt::format_to(m_sql, format.str(), static_cast<v.get_type()>(v));
                        }

                        if (argLen > 0)
                        {
                            fmt::format_to(m_sql," ) " );
                        }
                        return *this;
                    }

                template <typename ... Args>
                    DBQuery & insert_or_update(const std::string & tbName ,const Args & ... args)
                    {
                        clear();
                        m_bUpset = true;
                        int argLen = sizeof ...(Args);
                        m_table = tbName;
                        fmt::format_to(m_sql,"insert into {} " ,tbName);
                        if (argLen > 0 )
                        {
                            fmt::format_to(m_sql, " ( ") ;
                        }
                        std::stringstream format;
                        std::stringstream duplicate;
                        for (int i  = 0; i < argLen ; i++)
                        {
                            if (i < argLen -1 ) {
                                format<< " {}, ";
                                duplicate<<" {" << i << "}=values({" << i << "}),";
                            }
                            else {
                                format<< " {} ";
                                duplicate<<" {" << i << "}=values({" << i << "})";
                            }
                        }
                        fmt::format_to(m_sql,format.str(),args...);
                        fmt::format_to(m_upsetDupSql, duplicate.str(),args...);

                        if (argLen > 0)
                        {
                            fmt::format_to(m_sql," ) " );
                        }
                        return *this;
                    }

                template <typename ... Args>
                    DBQuery & update (const std::string & tbName,const Args & ... args)
                    {
                        clear();
                        int argLen = sizeof ...(Args);
                        m_table = tbName ;
                        fmt::format_to(m_sql," update {} ",tbName);
                        std::stringstream format;
                        for (int i  = 0; i  < argLen  ; i++)
                        {
                            if (i < argLen -1 ) {
                                format<< " {}, ";
                            }
                            else {
                                format<< " {} ";
                            }
                        }

                        fmt::format_to(m_sql,format.str(),args...);
                        dlog("sql: %s",fmt::to_string(m_sql).c_str());
                        return *this;
                    }

                DBQuery& set(const std::string& term) {

                    set_item_count ++;
                    if (set_item_count <= 1)
                    {
                        fmt::format_to(m_sql," set {} ",term);
                    }
                    else {
                        fmt::format_to(m_sql," , {} ",term);
                    }

                    return *this;
                }

                template<class T>
                    DBQuery& set(const std::string & key, T val )
                    {
                        set_item_count ++;
                        if (set_item_count <= 1)
                        {
                            fmt::format_to(m_sql," set {} = {} ",key,val);
                        }
                        else {
                            fmt::format_to(m_sql," , {} = {} ",key,val);
                        }
                        return *this;
                    }

                DBQuery & del(const std::string &tbName)
                {
                    clear();
                    fmt::format_to(m_sql," delete from {} ",tbName);
                    dlog("sql: %s",fmt::to_string(m_sql).c_str());
                    return *this;
                }

                DBQuery& set(const std::string & key, const char *  val)
                {
                    return this->set(key,std::string(val));
                }

                DBQuery& set(const std::string & key, const std::string & val)
                {
                    set_item_count ++;
                    if (set_item_count <= 1)
                    {
                        fmt::format_to(m_sql," set {} = \"{}\" ",key,gdp::db::EscapeString(val));
                    }
                    else {
                        fmt::format_to(m_sql," , {} = \"{}\" ",key,gdp::db::EscapeString(val));
                    }
                    return *this;
                }

                template <class T>
                    std::string printarg(T t)
                    {
                        return fmt::format("{}",t);
                    }

                std::string printarg(const char *  t)
                {
                    return fmt::format("\"{}\"",gdp::db::EscapeString(t));
                }

                std::string printarg(const std::string& t )
                {
                    return fmt::format("\"{}\"",gdp::db::EscapeString(t));
                }


                template <class T>
                    static std::string escape_param(T t)
                    {
                        return fmt::format("{}",t);
                    }

                static std::string escape_param(const char *  t)
                {
                    return fmt::format("{}",gdp::db::EscapeString(t));
                }

                static std::string escape_param(const std::string& t )
                {
                    return fmt::format("{}",gdp::db::EscapeString(t));
                }

                template <typename ... Args>
                    DBQuery & values(const Args & ... args)
                    {
                        int argLen = sizeof ...(Args);
                        fmt::format_to(m_sql," values ( ");

                        std::stringstream format;
                        for (int i  = 0; i < argLen  ; i++)
                        {
                            if (i < argLen -1 )
                            {
                                format<< " {}, ";
                            }
                            else {
                                format<< " {} ";
                            }
                        }
                        fmt::format_to(m_sql,format.str(),printarg(args)...);
                        fmt::format_to(m_sql," ) " );

                        if(m_bUpset)
                        {
                            fmt::format_to(m_sql," on duplicate key update " );
                            fmt::format_to(m_sql,fmt::to_string(m_upsetDupSql));
                        }
                        return *this;
                    }



                template <typename ... Args>
                    DBQuery & select(const Args & ... args)
                    {
                        clear();
                        int argLen = sizeof ...(Args);

                        fmt::format_to(m_sql,"select ");
                        if (argLen > 0)
                        {
                            std::stringstream format;
                            for (int i  = 0; i   < argLen  ; i++)
                            {
                                if (i < argLen -1 )
                                {
                                    format<< " {}, ";
                                }
                                else {
                                    format<< " {} ";
                                }
                            }
                            fmt::format_to(m_sql,format.str(),args...);
                        }
                        else
                        {

                            fmt::format_to(m_sql," * ");
                        }
                        return *this;
                    }

                DBQuery & select(const std::string & selData)
                {
                    clear();
                    fmt::format_to(m_sql,"select {} ",selData);
                    return *this;
                }

                DBQuery & from(const std::string & tbName)
                {
                    m_table = tbName;
                    fmt::format_to(m_sql," from {} ",tbName);
                    return *this;
                }

                DBQuery & where(SelfHandler self)
                {
                    where_levels ++;
                    if (where_levels >= MAX_NEST_LEVEL)
                    {
                        elog("exceed the max nest level");
                        return *this;
                    }
                    where_level_count[where_levels] = 0;
                    if (where_level_count[where_levels -1 ] > 0)
                    {
                        fmt::format_to(m_sql," and ");
                    }

                    if (where_levels == 1 && where_level_count[0] == 0)
                    {
                        fmt::format_to(m_sql," where ");
                        where_level_count[0]++;
                    }

                    fmt::format_to(m_sql," ( ");
                    self(*this);
                    fmt::format_to(m_sql,"  ) ");
                    where_levels --;

                    return *this;
                }

                template <typename T>
                    DBQuery & where(const std::string & key ,  T   term)
                    {
                        return where<T>(key,"=",term);
                    }

                DBQuery & where(const std::string & key , const char * term )
                {
                    return this->where(key," = ",term);
                }

                template <typename T>
                    DBQuery & where(const std::string & key , const std::string & op, const T   & term)
                    {
                        std::string termStr = fmt::format(" {} {} {} ", key , op, printarg(term));
                        this->_where(termStr);
                        return *this;
                    }

                //DBQuery & where(const std::string & key , const std::string & op, const std::string & term)
                //{
                //    std::string termStr = fmt::format("{} {} \"{}\"", key , op, gdp::db::EscapeString(term));
                //    dlog("where term is %s",termStr);
                //    this->_where(termStr);
                //    return *this;
                //}

                template <typename T>
                    DBQuery & where_raw(T term)
                    {
                        std::string termStr = fmt::format(" {} ",term);
                        this->_where(termStr);
                        return *this;
                    }

                void _where(const std::string & term)
                {
                    where_level_count[where_levels] ++;
                    if (where_levels > 0)
                    {
                        if (where_level_count[where_levels] > 1)
                        {
                            fmt::format_to(m_sql," and {} " ,term);
                        }
                        else
                        {
                            fmt::format_to(m_sql," {} " ,term);
                        }
                    }
                    else
                    {
                        if (where_level_count[where_levels] > 1)
                        {
                            fmt::format_to(m_sql," and {} " ,term);
                        }
                        else
                        {
                            fmt::format_to(m_sql," where {} " ,term);
                        }
                    }
                }
                template<typename T>
                    DBQuery & or_where(const std::string & key ,const std::string & op , const T&   term)
                    {
                        where_level_count[where_levels] ++;
                        std::string termStr = fmt::format(" {} {} {} ", key , op, printarg(term));
                        fmt::format_to(m_sql, " or {} ", termStr);
                        return *this;
                    }

                template<typename T>
                    DBQuery & or_where(const std::string & key , const T & term)
                    {
                        return or_where<T>(key, "=", term);
                    }


                DBQuery & or_where(SelfHandler self)
                {
                    where_levels ++;
                    if (where_levels >= MAX_NEST_LEVEL)
                    {
                        elog("exceed the max nest level");
                        return *this;
                    }
                    where_level_count[where_levels] = 0;
                    if (where_level_count[where_levels -1 ] > 0)
                    {
                        fmt::format_to(m_sql," or ");
                    }

                    if (where_levels > 1)
                    {
                        fmt::format_to(m_sql," ( ");
                        self(*this);
                        fmt::format_to(m_sql,"  ) ");
                        where_levels --;
                    }
                    else {

                        fmt::format_to(m_sql," where ");
                        self(*this);
                    }
                    return *this;
                }

                DBQuery & limit( unsigned int count)
                {
                    fmt::format_to(m_sql," limit {} " ,count);
                    return *this;
                }

                DBQuery & limit(unsigned int offset , unsigned int count)
                {
                    fmt::format_to(m_sql," limit {}, {} " ,offset , count);
                    return *this;
                }

                DBQuery & group_by (const std::string & col)
                {
                    fmt::format_to(m_sql," group by {} " ,col);
                    return *this;
                }


                DBQuery & order_by(const std::string & col, const std::string & type = "asc" )
                {
                    fmt::format_to(m_sql," order by {} {} " ,col,type );
                    return *this;
                }

                DBQuery & join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    return inner_join(tb,lId,op,rId);
                }

                DBQuery & inner_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    fmt::format_to(m_sql," inner join {} on {} {} {} " ,tb, lId,op,rId);
                    return *this;
                }

                DBQuery & left_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    fmt::format_to(m_sql," left join {} on {} {} {} " ,tb, lId,op,rId);
                    return *this;
                }

                DBQuery & right_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    fmt::format_to(m_sql," right join {} on {} {} {} " ,tb, lId,op,rId);
                    return *this;
                }

                std::string sql() const
                {
                    return fmt::to_string(m_sql);
                }

                /*********format syntax***************
                format_spec ::=  [[fill]align][sign]["#"]["0"][width]["." precision][type]
                fill        ::=  <a character other than '{', '}' or '\0'>
                align       ::=  "<" | ">" | "=" | "^"
                sign        ::=  "+" | "-" | " "
                width       ::=  integer | "{" arg_id "}"
                precision   ::=  integer | "{" arg_id "}"
                type        ::=  int_type | "a" | "A" | "c" | "e" | "E" | "f" | "F" | "g" | "G" | "p" | "s"
                int_type    ::=  "b" | "B" | "d" | "n" | "o" | "x" | "X"
                */
                template<typename ... Args>
                    static std::string format(const std::string & sql, const Args & ... args ) {
                        return fmt::format(sql,escape_param(args)...);
                    }

                void clear()
                {
                    m_sql = fmt::memory_buffer();
                    m_upsetDupSql = fmt::memory_buffer();
                    for(uint32_t i =0; i < MAX_NEST_LEVEL; i ++)
                    {
                        where_level_count[i] = 0 ;
                    }
                    set_item_count = 0;
                    where_levels = 0;
                }

            private:

                std::vector<std::string > definitions;
                std::string  m_table;
                fmt::memory_buffer m_sql ;
                uint32_t where_level_count[MAX_NEST_LEVEL] = {0};
                uint32_t where_levels = 0;
                uint32_t set_item_count = 0;
                bool m_bUpset = false;
                fmt::memory_buffer m_upsetDupSql;

        };

    }
}
