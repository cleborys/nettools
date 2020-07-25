#ifndef NETTOOLS_TLSENUMS_H
#define NETTOOLS_TLSENUMS_H

#include <cstdint>

enum HandshakeType : std::uint8_t {
  HandshakeType_client_hello = 1,
  HandshakeType_server_hello = 2,
};

enum CipherSuite : std::uint16_t {
  CipherSuite_TLS_AES_128_GCM_SHA256 = 0x1301,
};

enum ExtensionType : std::uint16_t {
  ExtensionType_server_name = 0,
  ExtensionType_supported_groups = 10,
  ExtensionType_signature_algorithms = 13,
  ExtensionType_supported_versions = 43,
  ExtensionType_key_share = 51,
};

enum SupportedGroups : std::uint16_t {
  SupportedGroups_x25519 = 0x001d,
};

enum SignatureAlgorithms : std::uint16_t {
  SignatureAlgorithms_ecdsa_secp256r1_sha256 = 0x0403,
};

#endif  // NETTOOLS_TLSENUMS_H
