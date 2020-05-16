#include "packets.h"
#include <arpa/inet.h>

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

std::ostream &operator<<(std::ostream &ostream,
                         const IPHeaderData &ip_header_data) {
  ostream << "IP Header Start\n"
          << "-------\n"
          << "source: " << ip_header_data.source << '\n'
          << "destination: " << ip_header_data.destination << '\n'
          << "ttl: " << ip_header_data.ttl << '\n'
          << "-------\n"
          << "IP Header End\n";

  return ostream;
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

RawBytes::RawBytes(size_t max_size)
    : max_size{max_size}, buffer{std::make_unique<char[]>(max_size)} {}

std::unique_ptr<const ICMPMessage> parse_at(const RawBytes &bytes,
                                            size_t start) {
  if (bytes.actual_size <= 0) {
    std::cerr << "Nothing to read, length: " << bytes.actual_size << '\n';
    return std::unique_ptr<ICMPMessage>(nullptr);
  }

  auto icmp_message{std::make_unique<ICMPMessage>()};

  auto buffer{bytes.get() + start};
  const iphdr *ip_header{reinterpret_cast<const iphdr *>(buffer)};

  in_addr source, dest;
  source.s_addr = ip_header->saddr;
  dest.s_addr = ip_header->daddr;

  icmp_message->ip_header.source = std::string(inet_ntoa(source));
  icmp_message->ip_header.destination = std::string(inet_ntoa(dest));
  icmp_message->ip_header.ttl = static_cast<int>(ip_header->ttl);
  icmp_message->ip_header.byte_length = static_cast<size_t>(ip_header->ihl * 4);
  icmp_message->ip_header.protocol = static_cast<int>(ip_header->protocol);

  const char *icmp_start{buffer + icmp_message->ip_header.byte_length};
  const icmphdr *icmp_header = reinterpret_cast<const icmphdr *>(icmp_start);

  icmp_message->msgtype = static_cast<int>(icmp_header->type);
  icmp_message->code = static_cast<int>(icmp_header->code);
  icmp_message->checksum = static_cast<int>(icmp_header->checksum);
  if (icmp_message->msgtype == ICMP_ECHO ||
      icmp_message->msgtype == ICMP_ECHOREPLY) {
    icmp_message->identifier =
        *reinterpret_cast<const int16_t *>(icmp_start + 4);
    icmp_message->sequence_number =
        *reinterpret_cast<const int16_t *>(icmp_start + 6);
  } else {
    icmp_message->identifier = -1;
    icmp_message->sequence_number = -1;
  }

  return icmp_message;
}
