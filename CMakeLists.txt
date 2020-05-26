cmake_minimum_required(VERSION 3.16)
project(nettools)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(traceroute traceroute/main.cpp packets.h traceroute/Traceroute.cpp traceroute/Traceroute.h packets.cpp Socket.cpp Socket.h)
target_link_libraries (traceroute ${CMAKE_THREAD_LIBS_INIT})

add_executable(dnstrace dnstrace/main.cpp Socket.h Socket.cpp packets.h packets.cpp dnstrace/dns.h dnstrace/dns.cpp dnstrace/DNSQuerier.cpp dnstrace/DNSQuerier.h)
