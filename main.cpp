#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>

int main() {
  int send_socket;
  send_socket = socket(PF_INET, SOCK_DGRAM, 0);
  if (send_socket < 0) {
    std::cerr << "Error opening socket for sending";
    return 1;
  }

  int receive_socket;
  receive_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (receive_socket < 0) {
    std::cerr << "Error opening socket for receiving";
    return 1;
  }

  const char ip_cstring[]{"172.217.19.78"};
  in_addr ip;
  if (!inet_aton(ip_cstring, &ip)) {
    std::cerr << "Failed to parse IP address " << ip_cstring << '\n';
    return 1;
  }

  hostent *remote_host;
  remote_host = gethostbyaddr((const void *)&ip, sizeof(ip), AF_INET);
  if (!remote_host) {
    std::cerr << "Failed to get host by address\n";
    return 1;
  }

  const char port_cstring[]{"80"};
  int port = atoi(port_cstring);

  sockaddr_in remote_address;
  memset((char *)&remote_address, 0, sizeof(remote_address));
  remote_address.sin_family = AF_INET;
  memcpy((void *)&remote_address.sin_addr, remote_host->h_addr,
         remote_host->h_length);
  remote_address.sin_port = htons(port);

  for (int ttl{0}; ttl < 20; ++ttl) {
    setsockopt(send_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

    char message[]{"hello"};
    if (sendto(send_socket, message, strlen(message), 0,
               (sockaddr *)&remote_address, sizeof(remote_address)) < 0) {
      std::cerr << "Failed to send\n";
      return 1;
    }
    std::cout << "sent\n";

    char buffer[2048];
    int received_length{};
    sockaddr_in received_addr;
    socklen_t received_addr_size{sizeof(received_addr)};
    std::cout << "Listening...\n";
    received_length = recvfrom(receive_socket, buffer, 2047, 0,
                               (sockaddr *)&received_addr, &received_addr_size);
    std::cout << "Received something of length " << received_length << ":\n";

    iphdr *ip_hdr = reinterpret_cast<iphdr *>(buffer);
    in_addr source, dest;
    source.s_addr = ip_hdr->saddr;
    dest.s_addr = ip_hdr->daddr;
    std::cout << "IP header length is " << ip_hdr->ihl * 4 << " bytes\n"
              << "ttl=" << static_cast<int>(ip_hdr->ttl) << '\n'
              << "source=" << inet_ntoa(source) << '\n'
              << "destination=" << inet_ntoa(dest) << '\n'
              << '\n';

    icmphdr *icmp_hdr =
        reinterpret_cast<icmphdr *>((char *)ip_hdr + (4 * ip_hdr->ihl));
    std::cout << "ICMP msgtype=" << static_cast<int>(icmp_hdr->type) << '\n'
              << "code=" << static_cast<int>(icmp_hdr->code) << '\n'
              << "checksum=" << static_cast<int>(icmp_hdr->checksum) << '\n'
              << '\n';
  }

  return 0;
}
