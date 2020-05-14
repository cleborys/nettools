#ifndef NETTOOLS__PACKETS_H_
#define NETTOOLS__PACKETS_H_

#include <string>

struct IPHeaderData {
  int ttl;
  std::string source;
  std::string destination;
};

struct ICMPMessage {
  IPHeaderData ip_header;
  int msgtype;
  int code;
  int checksum;
};

#endif  // NETTOOLS__PACKETS_H_
