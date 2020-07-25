#ifndef NETTOOLS_SRC_TLS_TLSRECORD_H_
#define NETTOOLS_SRC_TLS_TLSRECORD_H_

#include <cstdint>
#include "../IPPacket.h"
#include "../TCPSocket.h"

enum ContentType : std::uint8_t {
  ContentType_INVALID = 0,
  ContentType_CHANGE_CIPHER_SPEC = 20,
  ContentType_ALERT = 21,
  ContentType_HANDSHAKE = 22,
  ContentType_APPLICATION_DATA = 23
};

class TLSRecord {
  ContentType type;
  uint16_t legacy_record_version;
  RawBytes fragment;

 public:
  TLSRecord(ContentType type, std::uint16_t legacy_record_version,
            const RawBytes &payload);
  std::unique_ptr<RawBytes> get_raw();
  ContentType get_type() const { return type; };
  const RawBytes &get_fragment() const { return fragment; };
};

std::unique_ptr<TLSRecord> readTLSRecord(TCPSocket &socket);

#endif  // NETTOOLS_SRC_TLS_TLSRECORD_H_
