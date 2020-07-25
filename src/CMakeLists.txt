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
    crypto/Curve25519.cpp
    crypto/Curve25519.h
    crypto/FieldElement.cpp
    crypto/FieldElement.h
    dns/dns.h
    dns/dns.cpp
    dns/DNSQuerier.cpp
    dns/DNSQuerier.h
    tls/handshake.h
    tls/TLSHandshakeMessage.h
    tls/TLSHandshakeMessage.cpp
    tls/TLSRecord.h
    tls/TLSRecord.cpp
    tls/TLSExtension.cpp
    tls/TLSExtension.h
    tls/TLSEnums.h
)
add_library(libnettools ${LIBRARY_FILES})
target_link_libraries(libnettools ${CMAKE_THREAD_LIBS_INIT})

add_executable(traceroute traceroute/main.cpp)
target_link_libraries(traceroute libnettools)

add_executable(dns dns/main.cpp)
target_link_libraries(dns libnettools)

add_executable(tcp_client tcp_client/main.cpp)
target_link_libraries(tcp_client libnettools)

add_executable(tcp_server tcp_server/main.cpp)
target_link_libraries(tcp_server libnettools)

add_executable(tls_client tls/main.cpp)
target_link_libraries(tls_client libnettools)
