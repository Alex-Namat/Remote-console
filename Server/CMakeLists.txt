cmake_minimum_required(VERSION 3.8)
project(Server)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        main.cpp
        sync_tcp_server.cpp sync_tcp_server.hpp
        client_session.cpp client_session.hpp
        win_process.cpp win_process.hpp)

set (BOOST_COMPONETS
        system
        filesystem)

find_package(Boost COMPONENTS ${BOOST_COMPONETS})

include_directories(${Boost_INCLUDE_DIRS})
add_executable(Server ${SOURCE_FILES})
target_link_libraries(Server ${Boost_LIBRARIES} ws2_32)
