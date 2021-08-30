cmake_minimum_required(VERSION 3.8)
project(Client LANGUAGES CXX)

set(SOURCES
        main.cpp
        sync_tcp_client.cpp)

set(HEADERS
        sync_tcp_client.hpp)

set (BOOST_COMPONETS
        system)

find_package(Boost 1.64.0 COMPONENTS ${BOOST_COMPONETS})

add_executable(Client ${SOURCES} ${HEADERS})

set_target_properties(Client PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON)

target_include_directories(Client PRIVATE ${Boost_INCLUDE_DIRS})

if(WIN32)
        target_link_libraries(Client ${Boost_LIBRARIES} ws2_32)
else()
        target_link_libraries(Client ${Boost_LIBRARIES})
endif()