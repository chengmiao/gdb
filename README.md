# tinymysql


try to implement a very simple ,easy to use mysql client c++ lib . 


just like : 


myConn.db("mysql").table("test").select ( "name", "sex","age" ).where(" id ", "=", 1).where("name","=","test").order_by("id").get([](sql::ResultSet & ){


            }); 


with this line above , it executes  a sql : 
"select  name,  sex,  age  from test where   id  = 1   and name = "test"  order by id asc" 


more sql syntax need to be implemented.  It's complicate to work on the sql grammar, if you can help, help me finished. 

the prototype of the api is borrowed from php library laravel. 







# build it 

git submodule init 
git submodule update 
cmake . 
make 

