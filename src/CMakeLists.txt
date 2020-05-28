cmake_minimum_required(VERSION 3.10)
project(nettools)

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

set(LIBRARY_FILES
        IPPacket.cpp
        IPPacket.h
        Socket.cpp
        Socket.h
        ICMPMessage.cpp
        ICMPMessage.h
        traceroute/Traceroute.cpp
        traceroute/Traceroute.h
        dns/dns.h
        dns/dns.cpp
        dns/DNSQuerier.cpp
        dns/DNSQuerier.h
        )
add_library(libnettools ${LIBRARY_FILES})


add_executable(traceroute traceroute/main.cpp)
target_link_libraries (traceroute ${CMAKE_THREAD_LIBS_INIT} libnettools)

add_executable(dns dns/main.cpp )
target_link_libraries(dns libnettools)
