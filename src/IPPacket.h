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

  IPHeaderData(const iphdr &raw_header);
  IPHeaderData(const char *buffer);
};

struct RawBytes {
  std::unique_ptr<char[]> buffer;
  size_t max_size;
  size_t actual_size;

  RawBytes(size_t max_size);
  RawBytes(const std::string &hex_byte_string);
  char *get() { return buffer.get(); }
  const char *get() const { return buffer.get(); }
};

std::ostream &operator<<(std::ostream &ostream,
                         const IPHeaderData &ip_header_data);

#endif  // NETTOOLS__PACKETS_H_
