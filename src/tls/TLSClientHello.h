#ifndef NETTOOLS_SRC_TLS_TLSCLIENTHELLO_H_
#define NETTOOLS_SRC_TLS_TLSCLIENTHELLO_H_

#include <cstdint>
#include <vector>
#include "../IPPacket.h"

enum CipherSuite : uint16_t {
  CipherSuite_TLS_AES_128_GCM_SHA256 = 0x1301,
};

enum ExtensionType : uint16_t {
  ExtensionType_server_name = 0,
  ExtensionType_supported_groups = 10,
  ExtensionType_signature_algorithms = 13,
  ExtensionType_supported_versions = 43,
  ExtensionType_key_share = 51,
};

enum SupportedGroups : uint16_t {
  SupportedGroups_x25519 = 0x001d,
};

enum SignatureAlgorithms : uint16_t {
  SignatureAlgorithms_ecdsa_secp256r1_sha256 = 0x0403,
};

struct TLSExtension {
  ExtensionType type;
  RawBytes extension_data;

  TLSExtension(ExtensionType type, size_t data_length)
      : type{type}, extension_data{data_length} {}
};

class TLSClientHello {
  uint16_t legacy_version;
  uint8_t random[32]{};
  uint8_t legacy_session_id_length{0};
  std::vector<CipherSuite> cipher_suites;
  std::vector<uint8_t> legacy_compression_methods = {0};
  std::vector<TLSExtension> extensions;

 public:
  TLSClientHello();
  std::unique_ptr<RawBytes> get_raw() const;
};

#endif  // NETTOOLS_SRC_TLS_TLSCLIENTHELLO_H_
