#include "TLSHandshakeMessage.h"
#include <iomanip>

TLSHandshakeMessage::TLSHandshakeMessage(const KeyShare &key_share)
    : handshake_type{1},       // client hello
      legacy_version{0x0303},  // TLSv1.2 for compatibility
      cipher_suites{CipherSuite_TLS_AES_128_GCM_SHA256},
      legacy_compression_methods{0},
      extensions{} {
  extensions.push_back(TLSExtension(ExtensionType_supported_versions));
  extensions.push_back(TLSExtension(ExtensionType_supported_groups));
  extensions.push_back(TLSExtension(ExtensionType_signature_algorithms));
  extensions.push_back(TLSExtension(key_share));

  // NOT SECURE
  for (std::uint8_t *write_ptr{random}; write_ptr < random + 32; ++write_ptr) {
    *write_ptr = static_cast<std::uint8_t>(rand());
  }
}

std::unique_ptr<RawBytes> TLSHandshakeMessage::get_raw() const {
  auto return_raw_bytes{std::make_unique<RawBytes>(512)};

  return_raw_bytes->push_back(static_cast<char>(handshake_type));

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

  for (const std::uint8_t *read_ptr{random}; read_ptr < random + 32;
       ++read_ptr) {
    return_raw_bytes->push_back(static_cast<char>(*read_ptr));
  }

  return_raw_bytes->push_back(static_cast<char>(legacy_session_id_length));

  return_raw_bytes->push_back(
      static_cast<std::uint16_t>(2 * cipher_suites.size()));
  for (const CipherSuite suite : cipher_suites) {
    return_raw_bytes->push_back(static_cast<std::uint16_t>(suite));
  }

  return_raw_bytes->push_back(
      static_cast<char>(0x01));  // Compression Methods Length
  return_raw_bytes->push_back(static_cast<char>(0x00));  // No Compression

  return_raw_bytes->push_back(static_cast<std::uint16_t>(
      extension_length));  // Length of extensions block
  for (const TLSExtension &extension : extensions) {
    return_raw_bytes->push_back(static_cast<std::uint16_t>(extension.type));
    return_raw_bytes->append_from(extension.extension_data);
  }
  return return_raw_bytes;
}

TLSHandshakeMessage::TLSHandshakeMessage(const RawBytes &raw_message) {
  handshake_type = HandshakeType(raw_message.read_byte_at(0));
  // size_t length{static_cast<size_t>(raw_message.read_three_bytes_at(1))};
  legacy_version = raw_message.read_two_bytes_at(4);
  for (int i = 0; i < 32; ++i) {
    random[i] = raw_message.read_byte_at(6 + i);
  }
  std::uint8_t session_id_length{raw_message.read_byte_at(38)};
  cipher_suites.push_back(
      CipherSuite(raw_message.read_two_bytes_at(39 + session_id_length)));
  legacy_compression_methods.push_back(
      raw_message.read_byte_at(41 + session_id_length));

  size_t extension_length{
      raw_message.read_two_bytes_at(42 + session_id_length)};
  size_t extensions_start{static_cast<size_t>(44 + session_id_length)};
  size_t next_extension_start{extensions_start};
  while (next_extension_start < extensions_start + extension_length) {
    ExtensionType next_extension_type{
        ExtensionType(raw_message.read_two_bytes_at(next_extension_start))};
    size_t next_extension_length{
        raw_message.read_two_bytes_at(next_extension_start + 2)};
    extensions.push_back(
        TLSExtension(next_extension_type, next_extension_length));
    for (size_t i = 0; i < next_extension_length; ++i) {
      extensions.back().extension_data.push_back(static_cast<char>(
          raw_message.read_byte_at(next_extension_start + 2 + i)));
    }
    next_extension_start += next_extension_length;
  }
}

std::unique_ptr<RawBytes> TLSHandshakeMessage::find_key_share() const {
  bool is_client_message{handshake_type == HandshakeType_client_hello};
  for (auto &extension : extensions) {
    if (extension.type == ExtensionType_key_share) {
      return extension.get_key_share(is_client_message);
    }
  }
  throw std::runtime_error("No key share found");
}

std::ostream &operator<<(std::ostream &ostream,
                         const TLSHandshakeMessage &message) {
  ostream << "TLS Handshake Message\n"
          << " - type: " << message.handshake_type << '\n'
          << " - legacy_version: " << std::showbase << std::hex
          << message.legacy_version << '\n'
          << " - random:";
  for (int i = 0; i < 32; ++i) {
    ostream << ' ' << std::noshowbase << std::hex << std::setfill('0')
            << std::setw(2) << static_cast<int>(message.random[i]);
  }
  ostream << '\n'
          << " - session_id: "
          << static_cast<int>(message.legacy_session_id_length) << '\n'
          << " - number of cipher suites: " << message.cipher_suites.size()
          << '\n'
          << " - number of compression methods: "
          << message.legacy_compression_methods.size() << '\n'
          << " - number of extensions: " << message.extensions.size() << '\n';
  return ostream;
}
