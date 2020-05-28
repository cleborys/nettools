#ifndef NETTOOLS__SOCKET_H_
#define NETTOOLS__SOCKET_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <memory>
#include <string>
#include "IPPacket.h"

class Socket {
 public:
  void set_ttl(const int ttl);

 protected:
  Socket(int type, int protocol);
  virtual ~Socket();

  int m_socket{};
};

class DatagramSocket : public Socket {
 public:
  void send_string(const std::string &message,
                   const std::string &destination_ip, int destination_port);
  void send_raw(void *message_begin, size_t message_length,
                const std::string &destination_ip, int destination_port);

  std::unique_ptr<RawBytes> read_next();
  bool messages_available(int timeout_seconds = 1);

 protected:
  DatagramSocket(int type, int protocol) : Socket(type, protocol) {}
};

class UDPSocket : public DatagramSocket {
 public:
  UDPSocket() : DatagramSocket(SOCK_DGRAM, 0) {}
};

class ICMPSocket : public DatagramSocket {
 public:
  ICMPSocket() : DatagramSocket(SOCK_RAW, IPPROTO_ICMP) {}
};
#endif  // NETTOOLS__SOCKET_H_
