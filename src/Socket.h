#ifndef NETTOOLS__SOCKET_H_
#define NETTOOLS__SOCKET_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <memory>
#include <string>
#include "IPPacket.h"

class Socket {
 public:
  Socket(int type, int protocol);
  virtual ~Socket();

  void set_ttl(const int ttl);
  void send_string(std::string message, std::string destination_ip,
                   int destination_port);
  void send_raw(void *message_begin, size_t message_length,
                std::string destination_ip, int destination_port);

  std::unique_ptr<RawBytes> read_next();
  bool messages_available(int timeout_seconds = 1);

 protected:
  int m_socket{};
};

class UDPSocket : public Socket {
 public:
  UDPSocket() : Socket(SOCK_DGRAM, 0) {}
};

class ICMPSocket : public Socket {
 public:
  ICMPSocket() : Socket(SOCK_RAW, IPPROTO_ICMP) {}
};
#endif  // NETTOOLS__SOCKET_H_
