#include "ICMPMessage.h"
#include <arpa/inet.h>

ICMPMessage::ICMPMessage(const IPHeaderData &header_data, int msgtype, int code,
                         int checksum, int identifier, int sequence_number)
    : ip_header{header_data},
      msgtype{msgtype},
      code{code},
      checksum{checksum},
      identifier{identifier},
      sequence_number{sequence_number} {}

std::unique_ptr<const ICMPMessage> parse_icmp_at(const RawBytes &bytes,
                                                 size_t start) {
  if (bytes.actual_size <= 0) {
    std::cerr << "Nothing to read, length: " << bytes.actual_size << '\n';
    return std::unique_ptr<ICMPMessage>(nullptr);
  }

  const char *buffer{bytes.get() + start};
  IPHeaderData ip_header_data{buffer};
  if (ip_header_data.protocol != IPPROTO_ICMP) {
    std::cerr << "Wrong protocol: " << bytes.actual_size
              << ". Expected ICMP(1)\n";
    return std::unique_ptr<ICMPMessage>(nullptr);
  }

  const char *icmp_start{buffer + ip_header_data.byte_length};
  const icmphdr *icmp_header = reinterpret_cast<const icmphdr *>(icmp_start);

  int msgtype{static_cast<int>(icmp_header->type)};
  int code{static_cast<int>(icmp_header->code)};
  int checksum{static_cast<int>(icmp_header->checksum)};
  int identifier{-1};
  int sequence_number{-1};
  if (msgtype == ICMP_ECHO || msgtype == ICMP_ECHOREPLY) {
    identifier = *reinterpret_cast<const int16_t *>(icmp_start + 4);
    sequence_number = *reinterpret_cast<const int16_t *>(icmp_start + 6);
  }

  return std::make_unique<ICMPMessage>(ip_header_data, msgtype, code, checksum,
                                       identifier, sequence_number);
}

std::unique_ptr<icmp> generate_echo_request(int sequence_number,
                                            int identifier) {
  auto packet{std::make_unique<icmp>()};
  packet->icmp_type = ICMP_ECHO;
  packet->icmp_code = 0;
  packet->icmp_id = identifier;
  packet->icmp_seq = sequence_number;
  packet->icmp_cksum = 0;

  uint32_t checksum_accumulator{0};
  uint16_t *packet_raw{reinterpret_cast<uint16_t *>(packet.get())};
  for (int i{0}; i < 4; ++i) {
    checksum_accumulator += packet_raw[i];
  }
  checksum_accumulator =
      (checksum_accumulator >> 16) + (checksum_accumulator & 0xffff);
  checksum_accumulator += (checksum_accumulator >> 16);
  packet->icmp_cksum = ~(static_cast<short>(checksum_accumulator));

  return packet;
}

std::ostream &operator<<(std::ostream &ostream,
                         const ICMPMessage &icmp_message) {
  ostream << "ICMP Message Start\n"
          << "--------------\n"
          << icmp_message.ip_header << "messagetype=" << icmp_message.msgtype
          << '\n'
          << "code=" << icmp_message.code << '\n'
          << "checksum=" << icmp_message.checksum << '\n'
          << "identifier=" << icmp_message.identifier << '\n'
          << "sequence number=" << icmp_message.sequence_number << '\n'
          << "--------------\n"
          << "ICMP Message End\n";

  return ostream;
}
