#include "Socket.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

const int BUFFER_SIZE = 2048;

Socket::Socket(int type, int protocol) {
  m_socket = socket(AF_INET, type, protocol);
  if (m_socket < 0) {
    std::cerr << "Failed to create socket. Do you have appropriate rights?\n";
    throw std::runtime_error("Failed to create socket.");
  }
}

Socket::~Socket() {
  if (close(m_socket) < 0) {
    std::cerr << "Failed to close socket\n";
  }
}

void Socket::set_ttl(const int ttl) {
  if (setsockopt(m_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
    std::cerr << "Failed to set ttl to " << ttl << '\n';
  }
}

std::unique_ptr<sockaddr_in> Socket::parse_sockaddr(
    const std::string &destination_ip, int destination_port) {
  std::unique_ptr<sockaddr_in> parsed_address{std::make_unique<sockaddr_in>()};

  in_addr destination;
  if (!inet_aton(destination_ip.c_str(), &destination)) {
    std::cerr << "Failed to parse IP address " << destination_ip << '\n';
    throw std::runtime_error("Failed to parse IP address");
  }

  hostent *remote_host;
  remote_host = gethostbyaddr(static_cast<const void *>(&destination),
                              sizeof(destination), AF_INET);
  if (!remote_host) {
    std::cerr << "Failed to get host " << destination_ip << "by address\n";
    throw std::runtime_error("Failed to get host by address");
  }

  parsed_address->sin_family = AF_INET;
  memcpy(static_cast<void *>(&(parsed_address->sin_addr)), remote_host->h_addr,
         remote_host->h_length);
  parsed_address->sin_port = htons(destination_port);

  return parsed_address;
}

void DatagramSocket::send_string(const std::string &message,
                                 const std::string &destination_ip,
                                 int destination_port) {
  auto message_cstr = message.c_str();
  send_raw(&message_cstr, strlen(message_cstr), destination_ip,
           destination_port);
}

void DatagramSocket::send_raw(void *message_begin, size_t message_length,
                              const std::string &destination_ip,
                              int destination_port) {
  auto destination_sockaddr = parse_sockaddr(destination_ip, destination_port);

  if (sendto(m_socket, message_begin, message_length, 0,
             reinterpret_cast<sockaddr *>(destination_sockaddr.get()),
             sizeof(*destination_sockaddr)) < 0) {
    std::cerr << "Failed to send\n";
  }
}

std::unique_ptr<RawBytes> DatagramSocket::read_next() {
  std::unique_ptr<RawBytes> buffer{std::make_unique<RawBytes>(BUFFER_SIZE)};
  buffer->actual_size =
      recv(m_socket, buffer->get_buffer(), BUFFER_SIZE - 1, 0);
  return std::move(buffer);
}

bool DatagramSocket::messages_available(int timeout_seconds) {
  timeval timeout;
  timeout.tv_sec = timeout_seconds;
  timeout.tv_usec = 0;

  fd_set read_sockets;
  FD_SET(m_socket, &read_sockets);
  return (select(m_socket + 1, &read_sockets, nullptr, nullptr, &timeout) > 0);
}
