cmake_minimum_required(VERSION 3.10)

#Name project
project(URL_ABRVT)

#standart C++
set(CMAKE_CXX_STANDARD 17)

#project files
add_executable(abbreviated_url
	src/main.cpp
	src/Server.cpp
	src/URL.cpp
	src/Storage.cpp
)

#include files
target_include_directories(abbreviated_url PRIVATE include)

#Dependencies
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(SQLite3 REQUIRED)

target_link_libraries(abbreviated_url PRIVATE sqlite3 Boost::system Boost::thread)


