cmake_minimum_required(VERSION 3.17)
project(votify)

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(votify_gui ServerGui.cpp
        ServerGui.cpp
        ServerGui.cpp
        ServerGui.cpp
        ServerGui.cpp
        ServerGui.cpp
        ServerGui.cpp)

target_link_libraries(votify_gui ${GTK3_LIBRARIES})
