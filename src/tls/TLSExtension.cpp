#include "TLSExtension.h"
#include <map>

KeyShare::KeyShare(const FieldElement &field_element)
    : data{field_element.to_string()} {}

TLSExtension::TLSExtension(ExtensionType type,
                           std::size_t extension_data_length)
    : type{type}, extension_data{extension_data_length} {}

TLSExtension::TLSExtension(ExtensionType type)
    : type{type}, extension_data{64} {
  switch (type) {
    case ExtensionType_server_name:
      // empty
      break;
    case ExtensionType_supported_groups:
      make_supported_groups();
      break;
    case ExtensionType_signature_algorithms:
      make_signature_algorithms();
      break;
    case ExtensionType_supported_versions:
      make_supported_versions();
      break;
    case ExtensionType_key_share:
      throw std::runtime_error("Need to provide key_share.");
  }
}

TLSExtension::TLSExtension(const KeyShare &key_share)
    : type{ExtensionType_key_share}, extension_data{64} {
  extension_data.push_back(
      static_cast<std::uint16_t>(0x0026));  // length: 2 + 2 + 2 + 32
  extension_data.push_back(static_cast<std::uint16_t>(
      0x0024));  // length of client key share: 2 + 2 + 32
  extension_data.push_back(static_cast<std::uint16_t>(SupportedGroups_x25519));
  extension_data.push_back(
      static_cast<std::uint16_t>(0x0020));  // keyshare length: 32 bytes
  for (int i{0}; i < 16; ++i) {
    extension_data.push_back(key_share.data.read_two_bytes_at(2 * i));
  }
}

void TLSExtension::make_supported_groups() {
  extension_data.push_back(static_cast<std::uint16_t>(0x0004));  // length
  extension_data.push_back(
      static_cast<std::uint16_t>(0x0002));  // supported groups list length
  extension_data.push_back(static_cast<std::uint16_t>(SupportedGroups_x25519));
}

void TLSExtension::make_signature_algorithms() {
  extension_data.push_back(static_cast<std::uint16_t>(0x0004));  // length
  extension_data.push_back(
      static_cast<std::uint16_t>(0x0002));  // signature_algorithms list length
  extension_data.push_back(
      static_cast<std::uint16_t>(SignatureAlgorithms_ecdsa_secp256r1_sha256));
}
void TLSExtension::make_supported_versions() {
  extension_data.push_back(static_cast<std::uint16_t>(0x0003));  // length
  extension_data.push_back(
      static_cast<char>(0x02));  // supported versions list length
  extension_data.push_back(static_cast<std::uint16_t>(0x0304));  // TLSv1.3
}

std::unique_ptr<RawBytes> TLSExtension::get_key_share(bool from_client) const {
  if (type != ExtensionType_key_share) {
    throw std::runtime_error("Not a key_share extension");
  }

  std::size_t group_entry_position{2};
  if (from_client) {
    group_entry_position = 4;
  }

  std::uint16_t first_group_entry{
      extension_data.read_two_bytes_at(group_entry_position)};
  if (first_group_entry != SupportedGroups_x25519) {
    throw std::runtime_error("Unsupported group");
  }

  auto key_share{std::make_unique<RawBytes>(32)};
  for (std::size_t i{0}; i < 32; ++i) {
    key_share->push_back(static_cast<char>(
        extension_data.read_byte_at(group_entry_position + 4 + i)));
  }

  return key_share;
}
