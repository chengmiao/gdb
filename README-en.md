# db 


try to implement a very simple ,easy to use mysql client c++ lib . 

* v2.3 Release Notes:

upgrade fmtlib to v5.x

add support format method to build sql quickly 

support nest where syntax 



just like : 


```cpp
GDb  db("127.0.0.1",3306,"root","passwd","tinydb"); 
DBQuery query; 
query.insert_into("users","name", "age","sex","phone","address").values( name.c_str(),
    20+i,1,"18930878762","shanghai"); 
db.execute(query); 
```

with this line above , it executes  a sql : 
"select  name,  sex,  age  from test where   id  = 1   and name = "test"  order by id asc" 


more sql syntax need to be implemented.  It's complicate to work on the sql grammar, if you can help, help me finished. 








# build it 

git submodule init   <br> 
git submodule update   <br> 
cmake .   <br> 
make   <br>

