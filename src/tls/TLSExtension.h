#ifndef NETTOOLS_TLSEXTENSION_H
#define NETTOOLS_TLSEXTENSION_H

#include "../IPPacket.h"
#include "../crypto/FieldElement.h"
#include "TLSEnums.h"
#include "cstdint"

struct KeyShare {
  RawBytes data;

  KeyShare(const FieldElement &field_element);
};

class TLSExtension {
 private:
  void make_supported_groups();
  void make_signature_algorithms();
  void make_supported_versions();

 public:
  ExtensionType type;
  RawBytes extension_data;

  TLSExtension(
      ExtensionType type,
      std::size_t extension_data_length);  // initializes empty except for type
  TLSExtension(
      ExtensionType type);  // initializes with default data depending on type
  TLSExtension(const KeyShare &key_share);  // initializes Key_share extension
                                            // with known key_share

  std::unique_ptr<RawBytes> get_key_share(
      bool from_client) const;  // TODO: use polymorphism instead
};

#endif  // NETTOOLS_TLSEXTENSION_H
