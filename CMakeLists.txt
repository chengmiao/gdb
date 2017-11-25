cmake_minimum_required (VERSION 2.6)
project (tinynet)
# The version number.
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h

include_directories("/usr/local/mysql/connector-c++-8.0/include/")
include_directories(".")
include_directories("libs")
include_directories("libs/mysql-connector-cpp/")
include_directories("libs/fmt")
include_directories("../tinylog")
include_directories("../tinylog/include")
include_directories("detail")

if(APPLE)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++11-extensions -std=c++11 -stdlib=libc++  -g  -D NDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++11-extensions -std=c++11 -stdlib=libc++  -g  ")
elseif(UNIX)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -pthread  -D NDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g ")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -D NDEBUG")
endif()
 

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(libs/fmt)
#add_subdirectory(libs/fmt EXCLUDE_FROM_ALL)


#link_libraries(curl)
 
# add the executable
set(SRC_LIST  test.cpp ) 
add_executable(test ${SRC_LIST})

target_link_libraries(test PRIVATE fmt::fmt-header-only)
target_link_libraries(test mysqlcppconn)




