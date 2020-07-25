#include "TLSRecord.h"

TLSRecord::TLSRecord(ContentType type, uint16_t legacy_record_version,
                     const RawBytes &payload)
    : type{type},
      legacy_record_version{legacy_record_version},
      fragment{payload.actual_size} {
  fragment.copy_from(payload);
}

std::unique_ptr<RawBytes> TLSRecord::get_raw() {
  size_t buffer_length{5 + fragment.actual_size};
  auto return_bytes_ptr{std::make_unique<RawBytes>(buffer_length)};

  char *write_ptr{return_bytes_ptr->get_buffer()};

  *(write_ptr++) = type;  // Content type (1 byte)

  *(write_ptr++) = static_cast<char>((legacy_record_version >> 8) &
                                     0xff);  // Version first byte of two
  *(write_ptr++) = static_cast<char>((legacy_record_version >> 0) &
                                     0xff);  // Version second byte of two

  *(write_ptr++) = static_cast<char>((fragment.actual_size >> 8) &
                                     0xff);  // Length first byte of two
  *(write_ptr++) = static_cast<char>((fragment.actual_size >> 0) &
                                     0xff);  // Length second byte of two
  return_bytes_ptr->actual_size = 5;

  char *copy_ptr{fragment.get_buffer()};
  const char *const end_ptr{fragment.get_buffer() + fragment.actual_size};
  while (copy_ptr < end_ptr) {
    *(write_ptr++) = *(copy_ptr++);
  }
  return_bytes_ptr->actual_size += fragment.actual_size;

  return return_bytes_ptr;
}

std::unique_ptr<TLSRecord> readTLSRecord(TCPSocket &socket) {
  auto raw_info{socket.read_bytes(5)};
  std::uint16_t content_length{raw_info->read_two_bytes_at(3)};
  auto raw_content{socket.read_bytes(content_length)};
  return std::make_unique<TLSRecord>(ContentType(raw_info->read_byte_at(0)),
                                     raw_info->read_two_bytes_at(1),  // version
                                     *raw_content);
}
