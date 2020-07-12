#include "TLSClientHello.h"

TLSClientHello::TLSClientHello()
    : legacy_version{0x0303},  // TLSv1.2 for compatibility
      cipher_suites{CipherSuite_TLS_AES_128_GCM_SHA256},
      extensions{} {
  extensions.push_back(TLSExtension(ExtensionType_supported_versions, 10));
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0003));  // length
  extensions.back().extension_data.push_back(
      static_cast<char>(0x002));  // supported versions length
  extensions.back().extension_data.push_back(
      static_cast<uint16_t>(0x0304));  // TLSv1.3

  // NOT SECURE
  for (uint8_t *write_ptr{random}; write_ptr < random + 32; ++write_ptr) {
    *write_ptr = static_cast<uint8_t>(rand());
  }
}

std::unique_ptr<RawBytes> TLSClientHello::get_raw() const {
  auto return_raw_bytes{std::make_unique<RawBytes>(512)};

  return_raw_bytes->push_back(
      static_cast<char>(0x01));  // Handshake Type: Client Hello(1)
  unsigned int total_length{
      2     // legacy_version
      + 32  // random
      + 1   // session_id_length
      + 2   // cipher_suite_size
      + 2 * static_cast<unsigned int>(cipher_suites.size()) +
      2    // no compression
      + 9  // TODO: just supported versions extension
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
      static_cast<uint16_t>(0x0007));  // Extensions Length
  for (const TLSExtension &extension : extensions) {
    return_raw_bytes->push_back(static_cast<uint16_t>(extension.type));
    return_raw_bytes->append_from(extension.extension_data);
  }
  return return_raw_bytes;
}