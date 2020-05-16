#ifndef NETTOOLS__ICMPSOCKET_H_
#define NETTOOLS__ICMPSOCKET_H_
#include <iostream>
#include <memory>
#include "packets.h"

class ICMPSocket {
 public:
  ICMPSocket();
  std::unique_ptr<RawBytes> read_next();
  bool messages_available(int timeout_usec = 0);
  void send_raw(void *message_begin, size_t message_length,
                std::string destination_ip, int destination_port);
  void set_ttl(const int ttl);

 private:
  int m_socket;
};

#endif  // NETTOOLS__ICMPSOCKET_H_
