cmake_minimum_required(VERSION 3.27)
project(servidor)

set(CMAKE_CXX_STANDARD 17)

find_package(jsoncpp CONFIG REQUIRED)
find_package (glog 0.7.0 REQUIRED)

add_executable(servidor main.cpp
        playlist.cpp
        playlist.h
)

target_link_libraries(servidor PRIVATE JsonCpp::JsonCpp glog::glog uuid)
