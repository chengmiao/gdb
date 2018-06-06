project (gdb)
cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/include/mysql)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/mysql-connector-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/fmt)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DSPDLOG_FMT_EXTERNAL)

add_executable(gdb samples/test.cpp ${FMT_SRC})
target_link_libraries(gdb mysqlcppconn)
target_link_libraries(gdb fmt)
