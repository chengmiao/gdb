project (gdb)
cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/include/mysql)
include_directories(/usr/local/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/fmt)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/fmt/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/googletest/googletest/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -g -D NDEBUG")  

link_directories(/usr/local/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/googletest/googletest/lib)
add_definitions(-DSPDLOG_FMT_EXTERNAL)
add_definitions("-Wall -g")

add_executable(test samples/test.cpp ${FMT_SRC})
add_executable(ggtest samples/ggtest.cpp ${FMT_SRC})

target_link_libraries(ggtest mysqlclient)
target_link_libraries(ggtest fmt)
target_link_libraries(ggtest libgtest.a -lpthread -lm)

target_link_libraries(test mysqlclient)
target_link_libraries(test fmt)

#add_executable(bench samples/bench.cpp ${FMT_SRC})
#add_executable(dbstmt samples/stmt.cpp ${FMT_SRC})
#
#target_link_libraries(dbstmt mysqlclient)
#target_link_libraries(dbstmt fmt)
#
#target_link_libraries(bench mysqlclient)
#target_link_libraries(bench fmt)
#target_link_libraries(dbstmt fmt)
