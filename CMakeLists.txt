cmake_minimum_required(VERSION 3.27)
project(votify)

set(CMAKE_CXX_STANDARD 17)

add_executable(votify_main main.cpp
        server.h
        server.cpp)

add_executable(votify_server server.cpp)

