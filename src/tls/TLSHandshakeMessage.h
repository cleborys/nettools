#ifndef NETTOOLS_SRC_TLS_TLSCLIENTHELLO_H_
#define NETTOOLS_SRC_TLS_TLSCLIENTHELLO_H_

#include <cstdint>
#include <vector>
#include "../IPPacket.h"
#include "TLSEnums.h"
#include "TLSExtension.h"

class TLSHandshakeMessage {
  HandshakeType handshake_type;
  std::uint16_t legacy_version;
  std::uint8_t random[32]{};
  std::uint8_t legacy_session_id_length{0};
  std::vector<CipherSuite> cipher_suites;
  std::vector<std::uint8_t> legacy_compression_methods;
  std::vector<TLSExtension> extensions;

 public:
  TLSHandshakeMessage(const KeyShare& key_share);
  TLSHandshakeMessage(const RawBytes& raw_message);
  std::unique_ptr<RawBytes> get_raw() const;
  friend std::ostream& operator<<(std::ostream& ostream,
                                  const TLSHandshakeMessage& message);

  std::unique_ptr<RawBytes> find_key_share() const;
};

std::ostream& operator<<(std::ostream& ostream,
                         const TLSHandshakeMessage& message);
#endif  // NETTOOLS_SRC_TLS_TLSCLIENTHELLO_H_
