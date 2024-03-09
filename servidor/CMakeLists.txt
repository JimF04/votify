cmake_minimum_required(VERSION 3.27)
project(servidor)

set(CMAKE_CXX_STANDARD 17)

add_executable(servidor main.cpp
        playlist.cpp
        playlist.h
        server.cpp
        server.h
        ../paginated_list/main.cpp
        ../paginated_list/PaginatedList.h
)

target_link_libraries(servidor PRIVATE uuid)
