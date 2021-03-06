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

add_executable(dayserver-tcp-sync dayserver-tcp-sync.cpp)
target_link_libraries(dayserver-tcp-sync boost_system)

add_executable(dayserver-tcp-async dayserver-tcp-async.cpp)
target_link_libraries(dayserver-tcp-async boost_system)

add_executable(dayclient-udp-sync dayclient-udp-sync.cpp)
target_link_libraries(dayclient-udp-sync boost_system pthread)

add_executable(dayserver-udp-sync dayserver-udp-sync.cpp)
target_link_libraries(dayserver-udp-sync boost_system)

add_executable(dayserver-udp-async dayserver-udp-async.cpp)
target_link_libraries(dayserver-udp-async boost_system)

add_executable(dayclient-udp-async dayclient-udp-async.cpp)
target_link_libraries(dayclient-udp-async boost_system pthread)

add_executable(dayserver-comb-async dayserver-comb-async.cpp)
target_link_libraries(dayserver-comb-async boost_system)
