cmake_minimum_required(VERSION 3.22)
project(servidor)

set(CMAKE_CXX_STANDARD 17)

find_package(jsoncpp CONFIG REQUIRED)
find_package (glog 0.7.0 REQUIRED)
find_package(unofficial-inih CONFIG REQUIRED)

add_executable(servidor main.cpp
        playlist.cpp
        playlist.h
        server.cpp
        server.h
        miniaudio.h
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

target_link_libraries(servidor PRIVATE JsonCpp::JsonCpp uuid ${GTK3_LIBRARIES} glog::glog unofficial::inih::inireader)
