#ifndef NETTOOLS_SRC_TCPSOCKET_H_
#define NETTOOLS_SRC_TCPSOCKET_H_

#include "Socket.h"

class TCPSocket : public Socket {
 public:
  TCPSocket() : Socket(SOCK_STREAM, 0) {}

  void connect(const std::string& destination_ip, int destination_port);
  void send_cstring(const char* cstring);
  void send_raw_bytes(const RawBytes& raw_bytes);
  TCPSocket& operator<<(const std::string& message);

  std::unique_ptr<RawBytes> read_bytes(size_t max_bytes_read);
};

#endif  // NETTOOLS_SRC_TCPSOCKET_H_
