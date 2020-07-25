#include "TCPSocket.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

void TCPSocket::connect(const std::string &destination_ip,
                        int destination_port) {
  auto destination_sockaddr = parse_sockaddr(destination_ip, destination_port);
  if (::connect(m_socket,
                reinterpret_cast<sockaddr *>(destination_sockaddr.get()),
                sizeof(*destination_sockaddr)) != 0) {
    std::cerr << "Failed to connect to host " << destination_ip << " on port "
              << destination_port << '\n';
    return;
  }
}

void TCPSocket::send_cstring(const char *cstring) {
  if (write(m_socket, cstring, strlen(cstring)) < 0) {
    std::cerr << "Failed to write to socket.\n";
  }
}

void TCPSocket::send_raw_bytes(const RawBytes &raw_bytes) {
  if (write(m_socket, raw_bytes.get_buffer(), raw_bytes.actual_size) < 0) {
    std::cerr << "Failed to write to socket.\n";
  }
}

TCPSocket &TCPSocket::operator<<(const std::string &message) {
  const char *cstring{message.c_str()};
  send_cstring(cstring);
  return *this;
}

std::unique_ptr<RawBytes> TCPSocket::read_bytes(size_t max_bytes_read) {
  auto bytes_ptr{std::make_unique<RawBytes>(max_bytes_read)};
  bytes_ptr->actual_size =
      read(m_socket, bytes_ptr->get_buffer(), max_bytes_read);
  return bytes_ptr;
}
