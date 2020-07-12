#include "IPPacket.h"
#include <arpa/inet.h>
#include <iomanip>
#include <sstream>

IPHeaderData::IPHeaderData(const iphdr &raw_header)
    : ttl{static_cast<int>(raw_header.ttl)},
      protocol{static_cast<int>(raw_header.protocol)},
      byte_length{static_cast<size_t>(raw_header.ihl * 4)} {
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
    : buffer{std::make_unique<char[]>(max_size + 1)},
      max_size{max_size},
      actual_size{0} {}  // use one more byte to double as a c_string

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

void RawBytes::push_back(char byte) {
  if (actual_size + 1 > max_size) {
    throw std::runtime_error("out of bounds");
  }

  char *write_ptr{get_buffer() + actual_size};
  *write_ptr = byte;
  actual_size += 1;
}

void RawBytes::push_back(uint16_t two_bytes) {
  push_back(static_cast<char>((two_bytes & 0xff00) >> 8));
  push_back(static_cast<char>(two_bytes & 0x00ff));
}

void RawBytes::copy_from(const RawBytes &original) {
  if (original.actual_size > max_size) {
    throw std::runtime_error("length out of bounds to copy");
  }
  const char *read_ptr{original.get_buffer()};
  const char *end_ptr{original.get_buffer() + original.actual_size};
  actual_size = 0;
  while (read_ptr < end_ptr) {
    push_back(static_cast<char>(*read_ptr++));
  }
}

void RawBytes::append_from(const RawBytes &original) {
  if (actual_size + original.actual_size > max_size) {
    throw std::runtime_error("length out of bounds to append");
  }
  const char *read_ptr{original.get_buffer()};
  const char *end_ptr{original.get_buffer() + original.actual_size};
  while (read_ptr < end_ptr) {
    push_back(*read_ptr++);
  }
}

std::string RawBytes::to_hex_byte_string() const {
  std::ostringstream convert;

  const char *read_ptr{get_buffer()};
  const char *end_ptr{get_buffer() + actual_size};

  while (read_ptr < end_ptr) {
    convert << std::noshowbase << std::hex << std::setfill('0') << std::setw(2);
    convert << static_cast<int>(static_cast<unsigned char>(*(read_ptr++)))
            << ' ';
  }

  std::string return_string{convert.str()};
  if (return_string.size() > 0) {
    return_string.pop_back();
  }

  return return_string;
}
