#ifndef NETTOOLS__PACKETS_H_
#define NETTOOLS__PACKETS_H_

#include <netinet/ip_icmp.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>

struct IPHeaderData {
  int ttl;
  int protocol;
  size_t byte_length;
  std::string source;
  std::string destination;
};

struct ICMPMessage {
  IPHeaderData ip_header;
  int msgtype;
  int code;
  int checksum;
  int identifier{-1};
  int sequence_number{-1};
};

struct RawBytes {
  std::unique_ptr<char[]> buffer;
  size_t max_size;
  size_t actual_size;

  RawBytes(size_t max_size);
  char *get_bytes() { return buffer.get(); }
  const char *get() const { return buffer.get(); }
};

std::unique_ptr<const ICMPMessage> parse_at(const RawBytes &bytes,
                                            size_t start);

std::unique_ptr<icmp> generate_echo_request(int sequence_number,
                                            int identifier);

std::ostream &operator<<(std::ostream &ostream,
                         const ICMPMessage &icmp_message);
std::ostream &operator<<(std::ostream &ostream,
                         const IPHeaderData &ip_header_data);

#endif  // NETTOOLS__PACKETS_H_
