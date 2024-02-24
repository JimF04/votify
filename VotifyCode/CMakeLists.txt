cmake_minimum_required(VERSION 3.27)
project(VotifyCode)

set(CMAKE_CXX_STANDARD 17)

add_executable(VotifyCode main.cpp
        Server.cpp
        Server.h)
