cmake_minimum_required(VERSION 3.16)
project(nettools)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(nettools main.cpp packets.h Traceroute.cpp Traceroute.h packets.cpp Socket.cpp Socket.h)
target_link_libraries (nettools ${CMAKE_THREAD_LIBS_INIT})
