#include <IPPacket.h>
#include "dns/dns.h"
#include "gtest/gtest.h"

TEST(DNSTest, write_hostname) {
  std::string hostname_to_write{"www.google.com."};
  unsigned char buffer[32]{};

  write_hostname(hostname_to_write, buffer);

  std::string output_string{reinterpret_cast<const char *>(buffer)};
  std::string expected_output{"\3www\6google\3com"};

  EXPECT_EQ(output_string, expected_output);
}

TEST(DNSTest, write_hostname_not_ending_in_dot) {
  std::string hostname_to_write{"www.google.com"};
  unsigned char buffer[32]{};

  EXPECT_THROW(write_hostname(hostname_to_write, buffer), std::runtime_error);
}

TEST(DNSTest, read_hostname_without_jump) {
  RawBytes raw_bytes{
      "ee 4d 81 80 00 01 00 01 00 00 00 00 05 62 6f 72 79 73 02 64 6b 00 00 01 "
      "00 01 c0 0c 00 01 00 01 00 00 0e 0f 00 04 00 50 d4 2f e5 85"};
  auto buffer_start{reinterpret_cast<const unsigned char *>(raw_bytes.get())};
  auto name_start{buffer_start + 12};

  std::string name_output{};
  size_t advance_output{};

  advance_output = copy_dns_name(name_start, buffer_start, name_output);

  std::string expected_name{"borys.dk"};
  EXPECT_EQ(name_output, expected_name);
  EXPECT_EQ(advance_output, expected_name.size() + 2);
}
