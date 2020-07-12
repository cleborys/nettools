#include "TLSClientHello.h"

TLSClientHello::TLSClientHello()
    : legacy_version{0x0303},  // TLSv1.2 for compatibility
      cipher_suites{CipherSuite_TLS_AES_128_GCM_SHA256},
      extensions{} {
  extensions.push_back(TLSExtension(ExtensionType_supported_versions, 10));
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0003));  // length
  extensions.back().extension_data.push_back(
      static_cast<char>(0x02));  // supported versions list length
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0304));  // TLSv1.3

  extensions.push_back(TLSExtension(ExtensionType_supported_groups, 10));
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0004));  // length
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0002));  // supported groups list length
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(SupportedGroups_x25519));

  extensions.push_back(TLSExtension(ExtensionType_signature_algorithms, 10));
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0004));  // length
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0002));  // signature_algorithms list length
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(SignatureAlgorithms_ecdsa_secp256r1_sha256));

  extensions.push_back(TLSExtension(ExtensionType_key_share, 64));
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0026));  // length: 2 + 2 + 2 + 32
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0024));  // length of client key share: 2 + 2 + 32
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(SupportedGroups_x25519));
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0020));  // keyshare length: 32 bytes
  // NOT USEFUl
  for (int i{0}; i < 16; ++i) {
    extensions.back().extension_data.push_back(
        static_cast<uint16_t>(rand()));  // append two random bytes
  }

  // NOT SECURE
  for (uint8_t *write_ptr{random}; write_ptr < random + 32; ++write_ptr) {
    *write_ptr = static_cast<uint8_t>(rand());
  }
}

std::unique_ptr<RawBytes> TLSClientHello::get_raw() const {
  auto return_raw_bytes{std::make_unique<RawBytes>(512)};

  return_raw_bytes->push_back(
      static_cast<char>(0x01));  // Handshake Type: Client Hello(1)

  size_t extension_length{0};
  for (const TLSExtension &extension : extensions) {
    extension_length += 2 + extension.extension_data.actual_size;
  }

  unsigned int total_length{
      2     // legacy_version
      + 32  // random
      + 1   // session_id_length
      + 2   // cipher_suite_size
      + 2 * static_cast<unsigned int>(cipher_suites.size()) +
      2                                              // no compression: 0x0100
      + 2                                            // extension_block_length
      + static_cast<unsigned int>(extension_length)  // extension block
  };
  return_raw_bytes->push_back(
      static_cast<char>((total_length >> 16) & 0xff));  // length first byte
  return_raw_bytes->push_back(
      static_cast<char>((total_length >> 8) & 0xff));  // length second byte
  return_raw_bytes->push_back(
      static_cast<char>((total_length >> 0) & 0xff));  // length third byte

  return_raw_bytes->push_back(legacy_version);

  for (const uint8_t *read_ptr{random}; read_ptr < random + 32; ++read_ptr) {
    return_raw_bytes->push_back(static_cast<char>(*read_ptr));
  }

  return_raw_bytes->push_back(static_cast<char>(legacy_session_id_length));

  return_raw_bytes->push_back(static_cast<uint16_t>(2 * cipher_suites.size()));
  for (const CipherSuite suite : cipher_suites) {
    return_raw_bytes->push_back(static_cast<uint16_t>(suite));
  }

  return_raw_bytes->push_back(
      static_cast<char>(0x01));  // Compression Methods Length
  return_raw_bytes->push_back(static_cast<char>(0x00));  // No Compression

  return_raw_bytes->push_back(
      static_cast<uint16_t>(extension_length));  // Length of extensions block
  for (const TLSExtension &extension : extensions) {
    return_raw_bytes->push_back(static_cast<uint16_t>(extension.type));
    return_raw_bytes->append_from(extension.extension_data);
  }
  return return_raw_bytes;
}