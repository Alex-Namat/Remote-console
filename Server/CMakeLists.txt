cmake_minimum_required(VERSION 3.8)
project(Server LANGUAGES CXX)

set(SOURCES
        main.cpp
        sync_tcp_server.cpp 
        client_session.cpp)

set(HEADERS
        sync_tcp_server.hpp
        client_session.hpp)

set (BOOST_COMPONETS
        system
        filesystem)

find_package(Boost COMPONENTS ${BOOST_COMPONETS})

if(WIN32)
        set(SPEC_FILES 
                win_process.cpp win_process.hpp)
endif()

add_executable(Server ${SOURCES} ${HEADERS} ${SPEC_FILES})

set_target_properties(Server PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON)

target_include_directories(Server PRIVATE ${Boost_INCLUDE_DIRS})

if(WIN32)
        target_link_libraries(Server ${Boost_LIBRARIES} ws2_32)
else()
        target_link_libraries(Server ${Boost_LIBRARIES})
endif()
