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

TEST(DNSTest, read_hostname_with_jump) {
  RawBytes raw_bytes{
      "f4 96 34 87 2e c2 28 9e fc 0d 7a 49 08 00 45 00 "
      "04 ae 81 91 00 00 38 11 6c 79 c1 00 0e 81 c0 a8 "
      "00 0b 00 35 98 a6 04 9a 5c 95 14 a5 80 00 00 01 "
      "00 00 00 0f 00 1b 06 67 6f 6f 67 6c 65 03 63 6f "
      "6d 00 00 01 00 01 c0 13 00 02 00 01 00 02 a3 00 "
      "00 14 01 61 0c 67 74 6c 64 2d 73 65 72 76 65 72 "
      "73 03 6e 65 74 00 c0 1c 00 02 00 01 00 02 a3 00 "
      "00 04 01 62 c0 2a c0 1c 00 02 00 01 00 02 a3 00 "
      "00 04 01 63 c0 2a c0 1c 00 02 00 01 00 02 a3 00 "};
  auto buffer_start{reinterpret_cast<const unsigned char *>(raw_bytes.get()) +
                    42};
  auto name_start{buffer_start + 114 - 42};

  std::string name_output{};
  size_t advance_output{};
  size_t expected_advance_count{4};

  advance_output = copy_dns_name(name_start, buffer_start, name_output);

  std::string expected_name{"b.gtld-servers.net"};
  EXPECT_EQ(name_output, expected_name);
  EXPECT_EQ(advance_output, expected_advance_count);
}
