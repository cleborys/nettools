find_package(Threads)

set(LIBRARY_FILES
        IPPacket.cpp
        IPPacket.h
        Socket.cpp
        Socket.h
        TCPSocket.cpp
        TCPSocket.h
        TCPServerSocket.cpp
        TCPServerSocket.h
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
target_link_libraries(libnettools ${CMAKE_THREAD_LIBS_INIT})


add_executable(traceroute traceroute/main.cpp)
target_link_libraries(traceroute libnettools)

add_executable(dns dns/main.cpp )
target_link_libraries(dns libnettools)

add_executable(tcp_client tcp_client/main.cpp )
target_link_libraries(tcp_client libnettools)

add_executable(tcp_server tcp_server/main.cpp )
target_link_libraries(tcp_server libnettools)
