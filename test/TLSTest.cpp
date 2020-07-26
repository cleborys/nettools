#include <tls/TLSHandshakeMessage.h>
#include "gtest/gtest.h"
#include "tls/TLSRecord.h"

TEST(TLSRecordTest, get_raw) {
  RawBytes payload{"01 02 03 52 b9"};

  TLSRecord record{ContentType_ALERT, 769, std::move(payload)};

  std::unique_ptr<RawBytes> raw_record{record.get_raw()};

  EXPECT_EQ(raw_record->actual_size, 5 + payload.actual_size);
  // ContentType_ALERT == 21 == 0x15
  // legacy_version == 769 == 0x0301
  // length == 5 == 0x0005
  EXPECT_EQ(raw_record->to_hex_byte_string(), "15 03 01 00 05 01 02 03 52 b9");
}

TEST(TLSClientHelloTest, simple_hello) {
  KeyShare key_share{FieldElement(0)};
  TLSHandshakeMessage client_hello{key_share};
  auto payload_ptr{client_hello.get_raw()};
  auto byte_string{payload_ptr->to_hex_byte_string()};

  EXPECT_EQ(byte_string.substr(0, 2),
            "01");  // type
  EXPECT_EQ(byte_string.substr(12, 5),
            "03 03");  // protocol
  EXPECT_EQ(byte_string.substr(114, 2),
            "00");  // session id length
  EXPECT_EQ(byte_string.substr(117, 11),
            "00 02 13 01");  // one standard cipher suite
  EXPECT_EQ(byte_string.substr(129, 5),
            "01 00");  // one standard cipher suite
  EXPECT_EQ(byte_string.substr(141, 20),
            "00 2b 00 03 02 03 04");  // supported_versions only TLSv1.3
}
