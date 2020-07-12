#include "IPPacket.h"
#include "gtest/gtest.h"

TEST(IPPacketTest, parse_ip_header) {
  RawBytes raw_bytes{
      "45 00 00 30 00 00 00 00 34 01 b6 0a 08 08 08 08 c0 a8 00 0b"};
  IPHeaderData ip_header{raw_bytes.get_buffer()};

  EXPECT_EQ(ip_header.ttl, 52);
  EXPECT_EQ(ip_header.source, "8.8.8.8");
  EXPECT_EQ(ip_header.destination, "192.168.0.11");
  EXPECT_EQ(static_cast<int>(ip_header.byte_length), 20);
  EXPECT_EQ(ip_header.protocol, IPPROTO_ICMP);
}

TEST(IPPacketTest, raw_bytest_to_string) {
  RawBytes raw_bytes{
      "45 00 00 30 00 00 00 00 34 01 b6 0a 08 08 08 08 c0 a8 00 0b"};
  EXPECT_EQ(raw_bytes.to_hex_byte_string(),
            "45 00 00 30 00 00 00 00 34 01 b6 0a 08 08 08 08 c0 a8 00 0b");
}

TEST(IPPacketTest, raw_bytest_to_string_empty) {
  RawBytes raw_bytes{""};
  EXPECT_EQ(raw_bytes.to_hex_byte_string(), "");
}

TEST(IPPacketTest, raw_bytest_copy) {
  RawBytes raw_bytes{
      "45 00 00 30 00 00 00 00 34 01 b6 0a 08 08 08 08 c0 a8 00 0b"};

  RawBytes copy{raw_bytes.actual_size};
  copy.copy_from(raw_bytes);

  EXPECT_EQ(raw_bytes.actual_size, copy.actual_size);
  EXPECT_EQ(raw_bytes.max_size, copy.max_size);
  EXPECT_EQ(raw_bytes.to_hex_byte_string(),
            "45 00 00 30 00 00 00 00 34 01 b6 0a 08 08 08 08 c0 a8 00 0b");
  EXPECT_EQ(copy.to_hex_byte_string(),
            "45 00 00 30 00 00 00 00 34 01 b6 0a 08 08 08 08 c0 a8 00 0b");
}
