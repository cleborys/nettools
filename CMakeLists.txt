cmake_minimum_required(VERSION 3.16)
project(nettools)

set(CMAKE_CXX_STANDARD 14)

add_executable(nettools main.cpp packets.h Traceroute.cpp Traceroute.h packets.cpp Socket.cpp Socket.h)
