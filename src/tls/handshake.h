#ifndef NETTOOLS_SRC_TLS_HANDSHAKE_H_
#define NETTOOLS_SRC_TLS_HANDSHAKE_H_

#include <cstdint>
enum HandshakeType {
  HandshakeType_client_hello = 1,
  HandshakeType_server_hello = 2
};

struct ClientHello {
  int16_t protocol_version = 0x0303;
  char random[32];
  int legacy_session_id;
  int cipher_suites;
  int legacy_compression_methods;
};
#endif  // NETTOOLS_SRC_TLS_HANDSHAKE_H_
