#include "ICMPSocket.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
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

ICMPMessage ICMPSocket::read_next() {
  char buffer[BUFFER_SIZE]{};
  int received_length{};
  sockaddr_in received_sockaddr{};
  socklen_t received_sockaddr_length{sizeof(received_sockaddr)};

  received_length =
      recvfrom(m_socket, buffer, BUFFER_SIZE - 1, 0,
               (sockaddr *)(&received_sockaddr), &received_sockaddr_length);

  if (received_length <= 0) {
    std::cout << "Nothing to read, length: " << received_length << '\n';
    return ICMPMessage{};
  }

  iphdr *ip_header{reinterpret_cast<iphdr *>(buffer)};
  in_addr source, dest;
  source.s_addr = ip_header->saddr;
  dest.s_addr = ip_header->daddr;
  ICMPMessage icmp_message;

  icmp_message.ip_header.source = std::string(inet_ntoa(source));
  icmp_message.ip_header.destination = std::string(inet_ntoa(dest));
  icmp_message.ip_header.ttl = static_cast<int>(ip_header->ttl);

  icmphdr *icmp_header =
      reinterpret_cast<icmphdr *>((char *)ip_header + (4 * ip_header->ihl));

  icmp_message.msgtype = static_cast<int>(icmp_header->type);
  icmp_message.code = static_cast<int>(icmp_header->code);
  icmp_message.checksum = static_cast<int>(icmp_header->checksum);

  return icmp_message;
}

std::ostream &operator<<(std::ostream &ostream,
                         const ICMPMessage &icmp_message) {
  ostream << "ICMP Message Start\n"
          << "--------------\n"
          << icmp_message.ip_header << "messagetype=" << icmp_message.msgtype
          << '\n'
          << "code=" << icmp_message.code << '\n'
          << "checksum=" << icmp_message.checksum << '\n'
          << "--------------\n"
          << "ICMP Message End\n";

  return ostream;
}

std::ostream &operator<<(std::ostream &ostream,
                         const IPHeaderData &ip_header_data) {
  ostream << "IP Header Start\n"
          << "-------\n"
          << "source: " << ip_header_data.source << '\n'
          << "destination: " << ip_header_data.destination << '\n'
          << "ttl: " << ip_header_data.ttl << '\n'
          << "-------\n"
          << "IP Header End\n";

  return ostream;
}
