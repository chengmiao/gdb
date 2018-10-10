#pragma once 
#include "escape_string.h"
#include "resultset.hpp"
#include "logger.hpp"
#include <stack>

namespace gdp
{
    namespace db
    {
        class DBQuery
        {
            public:
                typedef std::function<void(DBQuery & ) > SelfHandler; 

                DBQuery(const std::string & tbName = "")
                {
                    m_table = tbName; 
                }

                DBQuery & into(const std::string & tbName)
                {
                    m_table = tbName; 
                    m_sql.write(" {} ",tbName); 
                    return *this; 
                }

                DBQuery & create(const std::string &tbName,bool check = false)
                {
                    if (check)
                    {
                        m_sql.write(" {} IF NOT EXISTS " , tbName); 
                    }
                    else 
                    {
                        m_sql.write(" {} " , tbName); 
                    }
                    return *this; 
                } 

                DBQuery & begin()
                {
                    m_sql << " ( " ; 
                    return *this; 
                }

                DBQuery & end()
                {
                    m_sql << " ) " ; 
                    return *this; 
                }

                DBQuery & def(const std::string  & key ,
                        const std::string & type, 
                        int length = 0 , 
                        const std::string & dft = "",bool inc = false){
                    
                    if (definitions.size() > 0)
                    {
                        m_sql << " , "; 
                    }

                    fmt::MemoryWriter defStr;
                    if (length >0)
                    {
                        defStr.write(" {} {}({}) {} ", key,type,length, inc ? " AUTO_INCREMENT " :"" ); 
                    }
                    else {
                        defStr.write(" {} {} {} ", key,type, inc ? " AUTO_INCREMENT " :"" ); 
                    }

                    m_sql << defStr.c_str(); 
                    return *this; 
                } 

                template <typename ... Args>
                    DBQuery & insert_into(const std::string & tbName ,const Args & ... args) 
                    {
                        clear(); 
                        int argLen = sizeof ...(Args); 
                        m_table = tbName; 
                        m_sql << " insert into " << tbName; 

                        if (argLen > 0 )
                        {
                            m_sql << " ( "; 
                        }
                        fmt::MemoryWriter format; 
                        for (int i  = 0; i   < argLen  ; i++)
                        {
                            if (i < argLen -1 ) {
                                format<< " {}, "; 
                            }
                            else {
                                format<< " {} "; 
                            }
                        }
                        m_sql.write(format.c_str(),args...); 
                        if (argLen > 0)
                        {
                            m_sql.write(" ) " ); 
                        }
                        return *this; 
                    }

                template <typename ... Args>
                    DBQuery & insert(const Args & ... args) 
                    {
                        clear(); 
                        int argLen = sizeof ...(Args); 
                        if (m_table.empty())
                        {
                            elog("invalid table name"); 
                        }
                        m_sql << " insert into " << m_table ; 

                        fmt::MemoryWriter format; 
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

                        if (argLen > 0 ) {
                            m_sql << " ( "; 
                            m_sql.write(format.c_str(),args...); 
                            m_sql.write(" ) " ); 
                        }
                        else {
                            m_sql.write(format.c_str(),args...); 
                        }

                        return *this; 
                    }

                template <typename ... Args>
                    DBQuery & update (const std::string & tbName,const Args & ... args) 
                    {
                        clear(); 
                        int argLen = sizeof ...(Args); 
                        m_table = tbName ; 
                        m_sql << " update " << tbName; 

                        fmt::MemoryWriter format; 
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

                        //dlog("format is %s ",format.c_str()); 
                        m_sql.write(format.c_str(),args...); 

                        dlog("sql: %s",m_sql.c_str()); 
                        return *this; 
                    }

                DBQuery& set(const std::string& term) {
                    fmt::MemoryWriter termStr; 
                    termStr.write(" {} ", term);  

                    set_item_count ++; 
                    if (set_item_count <= 1)
                    {
                        m_sql << " set " << termStr.c_str(); 
                    }
                    else {
                        m_sql << " , " << termStr.c_str(); 
                    }

                    return *this; 
                }

                template<class T> 
                    DBQuery& set(const std::string & key, T val )
                    {
                        fmt::MemoryWriter termStr; 
                        termStr.write(" {} = {} ", key ,  val);  

                        set_item_count ++; 
                        if (set_item_count <= 1)
                        {
                            m_sql << " set " << termStr.c_str(); 
                        }
                        else {
                            m_sql << " , " << termStr.c_str(); 
                        }

                        return *this; 
                    }

                DBQuery & del(const std::string &tbName)
                {
                    clear(); 
                    m_sql << "delete from " << tbName; 
                    dlog("sql: %s",m_sql.c_str()); 
                    return *this; 
                }

                DBQuery& set(const std::string & key, const char *  val)
                {
                    return this->set(key,std::string(val)); 
                }

                DBQuery& set(const std::string & key, const std::string & val)
                {
                    fmt::MemoryWriter termStr;
                    auto str = gdp::db::EscapeString(val);
                    termStr.write(" {} = \"{}\" ", key, str);

                    set_item_count ++; 
                    if (set_item_count <= 1)
                    {
                        m_sql << " set " << termStr.c_str(); 
                    }
                    else {
                        m_sql << " ," << termStr.c_str(); 
                    }
                    return *this; 
                }

                template <class T>
                    std::string printarg(T t)
                    {
                        fmt::MemoryWriter val ;
                        val << t ; 
                        return val.c_str() ; 
                    }

                std::string printarg(const char *  t)
                {
                    auto str = gdp::db::EscapeString(t);
                    return std::string("\"") + str +"\"";
                }

                std::string printarg(const std::string& t )
                {
                    auto str = gdp::db::EscapeString(t);
                    return std::string("\"") + str +"\"";
                }


                template <typename ... Args>
                    DBQuery & values(const Args & ... args) 
                    {
                        int argLen = sizeof ...(Args); 
                        m_sql << " values " << " ( "  ; 

                        fmt::MemoryWriter format; 
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
                        m_sql.write(format.c_str(),printarg(args)...); 
                        m_sql.write(" ) " ); 
                        return *this; 
                    }



                template <typename ... Args>
                    DBQuery & select(const Args & ... args) 
                    {
                        clear(); 
                        int argLen = sizeof ...(Args); 
                        m_sql << "select " ; 
                        if (argLen > 0)
                        {
                            fmt::MemoryWriter format; 
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
                            m_sql.write(format.c_str(),args...); 
                        }
                        else 
                        {
                            m_sql << " * " ; 
                        }
                        return *this; 
                    }

                DBQuery & select(const std::string & selData)
                {
                    clear(); 
                    m_sql.write("select {} ",selData); 
                    return *this; 
                }

                DBQuery & from(const std::string & tbName)
                {
                    m_table = tbName; 
                    m_sql.write(" from {} ",tbName); 
                    return *this; 
                }

                DBQuery & where(SelfHandler self)
                {
                    where_levels ++; 
                    where_level_count[where_levels] = 0; 
                    if (where_level_count[where_levels -1 ] > 0)
                    {
                        m_sql << " and  "; 
                    }
                    m_sql << " ( "; 
                    self(*this);
                    m_sql << " ) "; 
                    where_levels --; 
                    return *this; 
                }

                DBQuery & where(const std::string & key , const char * term )
                {
                    return this->where(key,"=",term); 
                }

                DBQuery & where(const std::string & key , const std::string & op, const char * term )
                {
                    return where(key,op,std::string(term)); 
                }

                DBQuery & where(const std::string & key , const std::string & op, const std::string & term)
                {
                    fmt::MemoryWriter termStr;
                    auto str = gdp::db::EscapeString(term);
                    termStr.write("{} {} \"{}\"", key , op, str);

                    this->_where(termStr.c_str()); 
                    

                    return *this; 
                }
                template <typename T> 
                    DBQuery & where_raw(T term)
                    {
                        fmt::MemoryWriter termStr; 
                        termStr.write(" {} ",  term);  
                        this->_where(termStr.c_str()); 
                       return *this; 
                    }

                void _where(const std::string & term)
                {
                    where_level_count[where_levels] ++; 
                    if (where_levels > 0)
                    {
                        if (where_level_count[where_levels] > 1)
                        {
                            m_sql.write("and {} " ,term); 
                        }
                        else 
                        {
                            m_sql.write(" {} " ,term); 
                        }
                    }
                    else 
                    {
                        if (where_level_count[where_levels] > 1)
                        {
                            m_sql.write("and {} " ,term); 
                        }
                        else 
                        {
                            m_sql.write(" where {} " ,term); 
                        }
                    }
                }

                template <typename T> 
                    DBQuery & where(const std::string & key ,  T   term)
                    {
                        return where<T>(key,"=",term); 
                    }

                template <typename T> 
                    DBQuery & where(const std::string & key , const std::string & op, T   term)
                    {
                        fmt::MemoryWriter termStr; 
                        termStr.write(" {} {} {} ", key , op, term);  

                        this->_where(termStr.c_str()); 

                        return *this; 
                    }

                template<typename T> 
                    DBQuery & or_where(const std::string & key ,const std::string & op , T  term)
                    {
                        fmt::MemoryWriter termStr; 
                        termStr.write(" {} {} {} ", key , op, term);  
                        where_level_count[where_levels] ++; 
                        m_sql.write( " or {} ", termStr.c_str()); 
                        return *this; 
                    }

                DBQuery & limit( unsigned int count)
                {
                    m_sql.write(" limit {} " ,count); 
                    return *this; 
                }

                DBQuery & limit(unsigned int offset , unsigned int count)
                {
                    m_sql.write(" limit {}, {} " ,offset , count); 
                    return *this; 
                }

                DBQuery & group_by (const std::string & col)
                {
                    m_sql.write(" group by {} " ,col); 
                    return *this; 
                }


                DBQuery & order_by(const std::string & col, const std::string & type = "asc" )
                {
                    m_sql.write(" order by {} {} " ,col,type ); 
                    return *this; 
                }

                DBQuery & join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    return inner_join(tb,lId,op,rId); 
                }

                DBQuery & inner_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    m_sql.write(" inner join {} on {} {} {} " ,tb, lId,op,rId); 
                    return *this; 
                }
                
                DBQuery & left_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    m_sql.write(" left join {} on {} {} {} " ,tb, lId,op,rId); 
                    return *this; 
                }

                DBQuery & right_join(const std::string & tb, const std::string & lId, const std::string & op , const std::string & rId)
                {
                    m_sql.write(" right join {} on {} {} {} " ,tb, lId,op,rId); 
                    return *this; 
                }

                const char *  sql() const 
                {
                    return m_sql.c_str(); 
                }

                void clear()
                {
                    m_sql.clear(); 
                }

            private:

                std::vector<std::string > definitions; 
                std::string  m_table; 
                fmt::MemoryWriter m_sql ;
                uint32_t where_level_count[32] = {0}; 
                uint32_t where_levels = 0; 
                uint32_t set_item_count = 0; 

        }; 

    }
}
