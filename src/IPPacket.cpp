#include "IPPacket.h"
#include <arpa/inet.h>
#include <sstream>

IPHeaderData::IPHeaderData(const iphdr &raw_header)
    : ttl{static_cast<int>(raw_header.ttl)},
      byte_length{static_cast<size_t>(raw_header.ihl * 4)},
      protocol{static_cast<int>(raw_header.protocol)} {
  in_addr source_addr, dest_addr;
  source_addr.s_addr = raw_header.saddr;
  dest_addr.s_addr = raw_header.daddr;

  source = std::string(inet_ntoa(source_addr));
  destination = std::string(inet_ntoa(dest_addr));
}

IPHeaderData::IPHeaderData(const char *buffer)
    : IPHeaderData(*reinterpret_cast<const iphdr *>(buffer)) {}

// LCOV_EXCL_START
std::ostream &operator<<(std::ostream &ostream,
                         const IPHeaderData &ip_header_data) {
  ostream << "IP Header Start\n"
          << "-------\n"
          << "source: " << ip_header_data.source << '\n'
          << "destination: " << ip_header_data.destination << '\n'
          << "ttl: " << ip_header_data.ttl << '\n'
          << "-------\n"
          << "IP Header End\n";

  return ostream;
}
// LCOV_EXCL_STOP

RawBytes::RawBytes(size_t max_size)
    : max_size{max_size},
      actual_size{0},
      buffer{std::make_unique<char[]>(max_size + 1)} {
}  // use one more byte to double as a c_string

RawBytes::RawBytes(const std::string &hex_byte_string)
    : RawBytes((hex_byte_string.size() + 1) / 3) {
  actual_size = max_size;

  char *byte_ptr{buffer.get()};
  std::istringstream convert{hex_byte_string};
  unsigned int next_byte{};
  while (convert >> std::hex >> next_byte) {
    *(byte_ptr) = next_byte;
    byte_ptr += 1;
  }
}
