#include "ICMPSocket.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

constexpr int BUFFER_SIZE = 2048;

ICMPSocket::ICMPSocket() {
  m_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (m_socket < 0) {
    std::cerr << "Failed to create ICMP socket\n";
    throw std::runtime_error(
        "ICMP socket not created. Do you have the rights to?");
  }
}

bool ICMPSocket::messages_available(int timeout_seconds) {
  timeval timeout;
  timeout.tv_sec = timeout_seconds;
  timeout.tv_usec = 0;

  fd_set read_sockets;
  FD_SET(m_socket, &read_sockets);
  return (select(m_socket + 1, &read_sockets, nullptr, nullptr, &timeout) > 0);
}

std::unique_ptr<RawBytes> ICMPSocket::read_next() {
  std::unique_ptr<RawBytes> buffer{std::make_unique<RawBytes>(BUFFER_SIZE)};
  buffer->actual_size = recv(m_socket, buffer->get_bytes(), BUFFER_SIZE - 1, 0);
  return std::move(buffer);
}

void ICMPSocket::send_raw(void *message_begin, size_t message_length,
                          std::string destination_ip, int destination_port) {
  in_addr destination;
  if (!inet_aton(destination_ip.c_str(), &destination)) {
    std::cerr << "Failed to parse IP address " << destination_ip << '\n';
    return;
  }

  hostent *remote_host;
  remote_host = gethostbyaddr(static_cast<const void *>(&destination),
                              sizeof(destination), AF_INET);
  if (!remote_host) {
    std::cerr << "Failed to get host " << destination_ip << "by address\n";
    return;
  }

  sockaddr_in destination_sockaddr{};
  destination_sockaddr.sin_family = AF_INET;
  memcpy(static_cast<void *>(&destination_sockaddr.sin_addr),
         remote_host->h_addr, remote_host->h_length);
  destination_sockaddr.sin_port = htons(destination_port);

  if (sendto(m_socket, message_begin, message_length, 0,
             (sockaddr *)&destination_sockaddr,
             sizeof(destination_sockaddr)) < 0) {
    std::cerr << "Failed to send\n";
  }
}

void ICMPSocket::set_ttl(const int ttl) {
  if (setsockopt(m_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
    std::cerr << "Failed to set ttl to " << ttl << '\n';
  }
}
