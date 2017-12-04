# GDB


简单的数据库封装库，使用c++11 简化sql语句的组装； 在数据库编程中，比较麻烦的是sql语句的组装，常见的方式是使用iostream 或printf方式进行组装，需要处理较多的类型转换或临时变量进行拼装。
此数据库封装库采用了C++11 语法，并将sql语句中的关键字通过函数的方式实现，简化编写sql的难度，减少声明的临时变量等。 

此数据库组件包含两个核心库，一个组装Sql语句，另一个管理sql连接，包括数据库连接池等。


* 此库依赖mysql官方提供的mysql-connector c++ 版本  [mysql Connector/C++]:https://dev.mysql.com/downloads/connector/cpp/
try to implement a very simple ,easy to use mysql client c++ lib . 


just like : 


```cpp
TinyMysql  tinydb("127.0.0.1",3306,"root","passwd","tinydb"); 
DBQueue queue; 
queue.table("users").insert("name", "age","sex","phone","address").values( name.c_str() ,20+i,1,"18930878762","shanghai"); 
tinydb.execute(queue); 
```

with this line above , it executes  a sql : 
"select  name,  sex,  age  from test where   id  = 1   and name = "test"  order by id asc" 


more sql syntax need to be implemented.  It's complicate to work on the sql grammar, if you can help, help me finished. 








# build it 

git submodule init   <br> 
git submodule update   <br> 
cmake .   <br> 
make   <br>

