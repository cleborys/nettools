#include "UDPSocket.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

UDPSocket::UDPSocket() {
  m_socket = socket(PF_INET, SOCK_DGRAM, 0);
  if (m_socket < 0) {
    std::cerr << "Failed to create UDP socket\n";
  }
}

UDPSocket::~UDPSocket() {
  if (close(m_socket) < 0) {
    std::cerr << "Failed to close UDP socket\n";
  }
}

void UDPSocket::send_string(std::string message, std::string destination_ip,
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

  auto message_cstr = message.c_str();
  if (sendto(m_socket, message_cstr, strlen(message_cstr), 0,
             (sockaddr *)&destination_sockaddr,
             sizeof(destination_sockaddr)) < 0) {
    std::cerr << "Failed to send\n";
  }
}
