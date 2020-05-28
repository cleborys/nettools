#include "ICMPMessage.h"
#include "IPPacket.h"
#include "gtest/gtest.h"

TEST(ICMPTest, parse_echo_reply) {
  RawBytes raw_bytes{
      "45 00 00 30 00 00 00 00 34 01 b6 0a 08 08 08 08 c0 a8 00 0b 00 00 05 b5 "
      "f0 4a 0a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00"};
  std::unique_ptr<const ICMPMessage> icmp_ptr{parse_icmp_at(raw_bytes, 0)};

  EXPECT_EQ(icmp_ptr->sequence_number, 10);
  EXPECT_EQ(icmp_ptr->msgtype, ICMP_ECHOREPLY);
  EXPECT_EQ(icmp_ptr->code, 0);
  EXPECT_EQ(icmp_ptr->identifier, 19184);
}

TEST(ICMPTest, parse_non_icmp_ipproto) {
  RawBytes raw_bytes{
      "45 00 00 30 00 00 00 00 34 02 b6 0a 08 08 08 08 c0 a8 00 0b 00 00 05 b5 "
      "f0 4a 0a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00"};
  std::unique_ptr<const ICMPMessage> icmp_ptr{parse_icmp_at(raw_bytes, 0)};

  EXPECT_FALSE(icmp_ptr);
}

TEST(ICMPTest, parse_empty) {
  RawBytes raw_bytes{""};
  std::unique_ptr<const ICMPMessage> icmp_ptr{parse_icmp_at(raw_bytes, 0)};

  EXPECT_FALSE(icmp_ptr);
}

TEST(ICMPTest, generate_echo_request) {
  int sequence_number{123};
  int identifier{456};

  auto echo_request_ptr{generate_echo_request(sequence_number, identifier)};

  int expected_checksum{64948};

  EXPECT_EQ(echo_request_ptr->icmp_type, ICMP_ECHO);
  EXPECT_EQ(echo_request_ptr->icmp_seq, sequence_number);
  EXPECT_EQ(echo_request_ptr->icmp_id, identifier);
  EXPECT_EQ(echo_request_ptr->icmp_cksum, expected_checksum);
}
