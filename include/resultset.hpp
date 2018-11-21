#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "mysql.h"
#include "logger.hpp"
#include <stdlib.h>


namespace gdp
{
    namespace db{


        class ResultSet : public std::enable_shared_from_this<ResultSet>
        {
            typedef std::unordered_map<std::string ,uint32_t > FieldMap;

            public:
            ResultSet(MYSQL_RES * r ):m_res(r)
            {
                m_field_num = mysql_num_fields(m_res);
                load_field_map();
            }
            ~ResultSet()
            {
                if (m_res)
                {
                    mysql_free_result(m_res);
                    m_res = nullptr;
                    m_row = nullptr;
                    m_field_lens = nullptr;
                }
            }

            std::shared_ptr<ResultSet> first()
            {
                if (m_res)
                {
                    if (!m_row)
                    {
                        m_row = mysql_fetch_row(this->m_res);
                        m_field_lens = mysql_fetch_lengths(this->m_res);
                    }
                    else
                    {
                        mysql_field_seek(this->m_res, 0);   //以当前m_res向后偏移 从第一个字段返回
                        m_row = mysql_fetch_row(this->m_res);
                        m_field_lens = mysql_fetch_lengths(this->m_res);
                    }
                    return shared_from_this();
                }
                return nullptr;
            }

            int32_t get_int32_at(uint32_t idx, int32_t default_val = 0)
            {
                if (idx >= m_field_num)
                {
                    elog("column index out of range");
                    return default_val;
                }
                if (m_row && m_row[idx])
                {
                    return atoi(m_row[idx]);
                }
                return default_val;
            }

            int64_t get_int64_at(uint32_t idx, int64_t default_val = 0)
            {
                if (idx >= m_field_num)
                {
                    elog("column index out of range");
                    return default_val;
                }
                if (m_row && m_row[idx])
                {
                    return atoll(m_row[idx]);
                }
                return default_val;
            }

            int32_t  get_int32(const std::string & key , int32_t default_val = 0)
            {
                if (m_row)
                {
                    auto field =  m_field_map.find(key);
                    if (field != m_field_map.end())
                    {
                        uint32_t idx = field->second;
                        if(!m_row[idx])
                            return default_val;
                        return atoi(m_row[idx]);
                    }
                }
                return default_val;
            }

            int64_t  get_int64(const std::string & key, int64_t default_val = 0 )
            {
                if (m_row)
                {
                    auto field =  m_field_map.find(key);
                    if (field != m_field_map.end())
                    {
                        uint32_t idx = field->second;
                        if(!m_row[idx])
                            return default_val;
                        return atoll(m_row[idx]);
                    }
                }
                return default_val;
            }

            std::string get_string(const std::string & key, const std::string & default_val = "")
            {
                if (m_row && m_field_lens)
                {
                    auto field =  m_field_map.find(key);
                    if (field != m_field_map.end())
                    {
                        uint32_t idx = field->second;
                        if(!m_row[idx])
                            return default_val;
                        return std::string(m_row[idx], m_field_lens[idx]);
                    }
                }
                return default_val;
            }

            std::string get_string_at(uint32_t idx, const std::string & default_val = "")
            {
                if (idx >= m_field_num)
                {
                    elog("column index out of range");
                    return default_val;
                }
                if (m_row && m_field_lens && m_row[idx])
                {
                    return std::string(m_row[idx], m_field_lens[idx]);
                }
                return default_val;
            }

            float get_float(const std::string & key, float default_val = 0)
            {
                if (m_row)
                {
                    auto field =  m_field_map.find(key);
                    if (field != m_field_map.end())
                    {
                        uint32_t idx = field->second;
                        if(!m_row[idx])
                            return default_val;
                        return std::stof(m_row[idx]);
                    }
                }
                return default_val;
            }

            float  get_float_at(uint32_t idx, float default_val = 0)
            {
                if (idx >= m_field_num)
                {
					elog("column index out of range" );
                    return default_val;
                }
                if (m_row && m_row[idx])
                {
                    return std::stof(m_row[idx]);
                }
                return default_val;
            }

            long long get_llong(const std::string & key, long long default_val = 0)
            {
                if (m_row)
                {
                    auto field =  m_field_map.find(key);
                    if (field != m_field_map.end())
                    {
                        uint32_t idx = field->second;
                        if(!m_row[idx])
                            return default_val;
                        return std::stoll(m_row[idx]);
                    }
                }
                return default_val;
            }

            float  get_llong_at(uint32_t idx, float default_val = 0)
            {
                if (idx >= m_field_num)
                {
                    elog("column index out of range");
                    return default_val;
                }
                if (m_row && m_row[idx])
                {
                    return std::stoll(m_row[idx]);
                }
                return default_val;
            }


            bool next()
            {
                if (m_res)
                {
                    m_row = mysql_fetch_row(m_res);
                    m_field_lens = mysql_fetch_lengths(m_res);

                    if (m_row)
                    {
                        return true;
                    }
                }
                return false;
            }

            my_ulonglong count()
            {
                return mysql_num_rows(m_res);
            }

            private:
            void load_field_map()
            {
                MYSQL_FIELD * fields = mysql_fetch_fields(m_res);   //获取每个字段的详细信息
                for(uint32_t i =0; i < m_field_num; i ++)
                {
                    m_field_map[fields[i].name ]  = i ;
                }
            }

            MYSQL_RES* m_res  = nullptr;    //结果集指针
            MYSQL_ROW  m_row  = nullptr;    //一行结果
            unsigned long * m_field_lens = nullptr;     //每个字段的长度
            FieldMap m_field_map;   //每个字段的名字
            uint32_t m_field_num = 0;   //字段个数
        };

        typedef std::shared_ptr<ResultSet> ResultSetPtr;
    }
}
