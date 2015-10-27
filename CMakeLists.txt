project(asio CXX)

cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(timer-sync timer-sync.cpp)
target_link_libraries(timer-sync boost_system)

add_executable(timer-async timer-async.cpp)
target_link_libraries(timer-async boost_system)

add_executable(timer-bind timer-bind.cpp)
target_link_libraries(timer-bind boost_system)

add_executable(timer-member timer-member.cpp)
target_link_libraries(timer-member boost_system)

add_executable(timer-thread timer-thread.cpp)
target_link_libraries(timer-thread boost_system boost_thread)

add_executable(dayclient-tcp-sync dayclient-tcp-sync.cpp)
target_link_libraries(dayclient-tcp-sync boost_system pthread)

add_executable(dayclient-tcp-async dayclient-tcp-async.cpp)
target_link_libraries(dayclient-tcp-async boost_system pthread)
