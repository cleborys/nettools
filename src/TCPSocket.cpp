#include "TCPSocket.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

void TCPSocket::connect(const std::string &destination_ip,
                        int destination_port) {
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

  if (::connect(m_socket, reinterpret_cast<sockaddr *>(&destination_sockaddr),
                sizeof(destination_sockaddr)) != 0) {
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

TCPSocket &TCPSocket::operator<<(const std::string &message) {
  const char *cstring{message.c_str()};
  send_cstring(cstring);
  return *this;
}

std::unique_ptr<RawBytes> TCPSocket::read_bytes(size_t max_bytes_read) {
  auto bytes_ptr{std::make_unique<RawBytes>(max_bytes_read)};
  read(m_socket, bytes_ptr->get_buffer(), max_bytes_read);
  bytes_ptr->actual_size = strlen(bytes_ptr->get_buffer());
  return std::move(bytes_ptr);
}
