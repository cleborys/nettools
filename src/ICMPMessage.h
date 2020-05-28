#ifndef NETTOOLS_SRC_ICMPMESSAGE_H_
#define NETTOOLS_SRC_ICMPMESSAGE_H_
#include "IPPacket.h"

const size_t ICMP_HEADER_LENGTH{8};

struct ICMPMessage {
  IPHeaderData ip_header;
  int msgtype;
  int code;
  int checksum;
  int identifier;
  int sequence_number;

  ICMPMessage(const IPHeaderData &header_data, int msgtype, int code,
              int checksum, int identifier, int sequence_number);
};

std::unique_ptr<const ICMPMessage> parse_icmp_at(const RawBytes &bytes,
                                                 size_t start);

std::unique_ptr<icmp> generate_echo_request(int sequence_number,
                                            int identifier);

std::ostream &operator<<(std::ostream &ostream,
                         const ICMPMessage &icmp_message);

#endif  // NETTOOLS_SRC_ICMPMESSAGE_H_
