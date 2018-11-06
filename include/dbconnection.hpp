#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include "resultset.hpp"

template<typename T>
struct SqlResult
{
    std::string errorString = std::string("");
    T resultVal;
};

namespace gdp {
namespace db {

class DBConnection {
 public:
  DBConnection() : m_mysql(nullptr) {}
  DBConnection(const DBConnection &) = delete;
  DBConnection &operator=(const DBConnection &) = delete;

  ~DBConnection() {
    m_connected = false;
    mysql_close(m_mysql);
    this->m_mysql = nullptr;
    mysql_thread_end();
  }

  SqlResult<bool> init(const std::string &db = "", const std::string &user = "root",
           const std::string &passwd = "",
           const std::string &host = "localhost", int port = 3306,
           unsigned int timeout = 10)

  {
    SqlResult<bool> retRes;
    retRes.resultVal = false;

    static std::once_flag flag;
    std::call_once(flag, []() { mysql_library_init(0, nullptr, nullptr); });

    m_mysql = mysql_init(nullptr);
    mysql_thread_init();

    //重连
    my_bool reconnect = 1;
    mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);
    //超时
    mysql_options(m_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    mysql_options(m_mysql, MYSQL_OPT_READ_TIMEOUT, &timeout);
    mysql_options(m_mysql, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
    //编码
    mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8");

    MYSQL *res = mysql_real_connect(m_mysql, host.c_str(), user.c_str(),
                                    passwd.c_str(), db.c_str(), port, NULL, 0);
    if (res) {
        m_connected = true;
        std::stringstream ss;
        ss << "connect to server " << user.c_str() << ":" << passwd.c_str() << "@" << host.c_str() << ":" << port << "success";
        retRes.errorString = ss.str();
        retRes.resultVal = true;
        dlog("connect to server %s:%s@%s:%d success", user.c_str(),
                passwd.c_str(), host.c_str(), port);

    } else {
        retRes.resultVal = false;
        std::stringstream ss;
        ss << "failed to connect to database error:[" << mysql_error(m_mysql) << "]";
        retRes.errorString = ss.str();
        elog("failed to connect to database error: %s\n", mysql_error(m_mysql));

    }
    return retRes;
  }

  SqlResult<bool> use(const std::string &db) {
      SqlResult<bool> retRes;
      retRes.resultVal = false;

      if (!this->is_connected()){
          retRes.resultVal = false;
          std::stringstream ss;
          ss << "execute query [select " << db.c_str() << "] error: not connected";
          retRes.errorString = ss.str();
          return retRes;

      }
      int ret = mysql_select_db(m_mysql, db.c_str());
      if (ret != 0) {
          elog("execute query  error %s\n", mysql_error(m_mysql));
          retRes.resultVal = false;
          std::stringstream ss;
          ss << "execute query [select " << db.c_str() << "] error: " << mysql_error(m_mysql);
          retRes.errorString = ss.str();
          return retRes;

      }

      retRes.resultVal = true;
      return retRes;

  }

  bool is_connected() { return m_connected;  }

  // execute sql without result
  SqlResult<bool> execute(const std::string &sql) {

      SqlResult<bool> retRes;
      retRes.resultVal = false;

      if (!this->is_connected()){
          retRes.resultVal = false;
          std::stringstream ss;
          ss << "execute query [" << sql.c_str() << "] error: not connected";
          retRes.errorString = ss.str();
          return retRes;
      }
      dlog("execute query :%s ", sql.c_str());
      int ret = mysql_real_query(m_mysql, sql.c_str(), sql.size());
      if (ret != 0) {
          elog("execute query error: %s\n", mysql_error(m_mysql));
          retRes.resultVal = false;
          std::stringstream ss;
          ss << "execute query [" << sql.c_str() << "] error: [" << mysql_error(m_mysql) << "]";
          retRes.errorString = ss.str();
          return retRes;
      }

      retRes.resultVal = true;
      return retRes;
  }

  SqlResult<ResultSetPtr> query(const std::string &sql) {

      SqlResult<ResultSetPtr> retRes;
      retRes.resultVal = nullptr;

      if (!this->is_connected()){
          retRes.resultVal = nullptr;
          std::stringstream ss;
          ss << "execute query [" << sql.c_str() << "] error: not connected";
          retRes.errorString = ss.str();
          return retRes;

      }
      int ret = mysql_real_query(m_mysql, sql.c_str(), sql.size());
      if (ret != 0) {
          elog("execute query  error %s\n", mysql_error(m_mysql));
          retRes.resultVal = nullptr;
          std::stringstream ss;
          ss << "execute query [" << sql.c_str() << "] error: [" << mysql_error(m_mysql) << "]";
          retRes.errorString = ss.str();
          return retRes;

      }

      MYSQL_RES *res = mysql_store_result(m_mysql);
      ResultSetPtr rset = std::make_shared<ResultSet>(res);
      retRes.resultVal = rset;
      return retRes;

  }

  my_ulonglong get_insert_id() {
    if (!this->is_connected()) return my_ulonglong(-1);
    return mysql_insert_id(m_mysql);
  }

  int get_affected_rows() {
    if (!this->is_connected()) return -1;
    return mysql_affected_rows(m_mysql);
  }

 private:
  bool m_connected = false;
  MYSQL *m_mysql = nullptr;
};

typedef std::shared_ptr<DBConnection> DBConnectionPtr;

}  // namespace db
}  // namespace gdp
