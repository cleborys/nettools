#ifndef NETTOOLS_DNSTRACE_DNS_H_
#define NETTOOLS_DNSTRACE_DNS_H_
#include <string>

struct DnsHeader {
  // first two bytes
  unsigned short id;  // identification number

  // third byte
  unsigned char recursion_desired : 1;     // recursion desired
  unsigned char truncated_message : 1;     // truncated message
  unsigned char authoritative_answer : 1;  // authoritive answer
  unsigned char opcode : 4;                // purpose of message
  unsigned char query_response_flag : 1;   // query/response flag

  // fourth byte
  unsigned char response_code : 4;        // response code
  unsigned char checking_disabled : 1;    // checking disabled
  unsigned char authenticated_data : 1;   // authenticated data
  unsigned char z_flag : 1;               // its z! reserved
  unsigned char recursion_available : 1;  // recursion available

  // bytes five through eight
  unsigned short question_count;   // number of question entries
  unsigned short answer_count;     // number of answer entries
  unsigned short authority_count;  // number of authority entries
  unsigned short additional_record_count;
};

struct QueryInfo {
  unsigned short query_type;
  unsigned short question_class;
};

#pragma pack(push, 1)
struct ResourceRecordHeader {
  unsigned short type;
  unsigned short record_class;
  unsigned int ttl;
  unsigned short resource_data_length;
};
#pragma pack(pop)

struct ResourceRecord {
  unsigned char *name;  // pointer to start of name entry
  ResourceRecordHeader header;
  unsigned char *resource_data;  // pointer to start of resource data
};

void write_hostname(const std::string &hostname, unsigned char *dns_query_ptr);

size_t copy_dns_name(const unsigned char *reader, const unsigned char *buffer,
                     std::string &out_name);

std::ostream &operator<<(std::ostream &ostream, const DnsHeader &dns_header);
std::ostream &operator<<(std::ostream &ostream,
                         const ResourceRecordHeader &record_header);

#endif  // NETTOOLS_DNSTRACE_DNS_H_
