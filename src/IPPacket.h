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

  char *get_buffer() { return buffer.get(); }
  const char *get_buffer() const { return buffer.get(); }

  void push_back(char byte);
  void push_back(uint16_t two_bytes);
  void copy_from(const RawBytes &original);
  void append_from(const RawBytes &original);

  std::string to_hex_byte_string() const;
};

std::ostream &operator<<(std::ostream &ostream,
                         const IPHeaderData &ip_header_data);

#endif  // NETTOOLS__PACKETS_H_
