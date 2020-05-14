#ifndef NETTOOLS__ICMPSOCKET_H_
#define NETTOOLS__ICMPSOCKET_H_
#include <iostream>
#include "packets.h"

class ICMPSocket {
 public:
  ICMPSocket();
  ICMPMessage read_next();

 private:
  int m_socket;
};

std::ostream &operator<<(std::ostream &ostream,
                         const ICMPMessage &icmp_message);
std::ostream &operator<<(std::ostream &ostream,
                         const IPHeaderData &ip_header_data);

#endif  // NETTOOLS__ICMPSOCKET_H_
