#include "IPPacket.h"
#include "gtest/gtest.h"

TEST(IPPacketTest, parse_ip_header) {
  RawBytes raw_bytes{
      "45 00 00 30 00 00 00 00 34 01 b6 0a 08 08 08 08 c0 a8 00 0b"};
  IPHeaderData ip_header{raw_bytes.get_buffer()};

  EXPECT_EQ(ip_header.ttl, 52);
  EXPECT_EQ(ip_header.source, "8.8.8.8");
  EXPECT_EQ(ip_header.destination, "192.168.0.11");
  EXPECT_EQ(ip_header.byte_length, 20);
  EXPECT_EQ(ip_header.protocol, IPPROTO_ICMP);
}
