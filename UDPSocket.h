#ifndef NETTOOLS__UDPSOCKET_H_
#define NETTOOLS__UDPSOCKET_H_

#include <string>

class UDPSocket {
 public:
  UDPSocket();
  ~UDPSocket();
  void set_ttl(const int ttl);
  void send_string(std::string message, std::string destination_ip,
                   int destination_port);
  void send_raw(void *message_begin, size_t message_length,
                std::string destination_ip, int destination_port);

 private:
  int m_socket{};
};

#endif  // NETTOOLS__UDPSOCKET_H_
