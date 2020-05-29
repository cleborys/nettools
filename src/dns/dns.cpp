#include "dns.h"
#include <netinet/in.h>
#include <iostream>

void write_hostname(const std::string &hostname, unsigned char *dns_query_ptr) {
  if (hostname.back() != '.') {
    std::cerr << "Hostname must end in '.'";
    throw std::runtime_error("Hostname must end in '.'");
  }

  int written_so_far{0};
  for (int lookahead{0}; lookahead < hostname.size(); ++lookahead) {
    if (hostname[lookahead] != '.') {
      continue;
    }
    // write the number of characters
    // like 3, 6, 3 in 3www6google3com
    *dns_query_ptr++ = lookahead - written_so_far;
    // write letters until current '.'
    while (written_so_far < lookahead) {
      *dns_query_ptr = hostname[written_so_far];
      dns_query_ptr += 1;
      written_so_far += 1;
    }
    written_so_far += 1;  // skip the current '.'
  }
  *dns_query_ptr++ = '\0';  // final null-terminator
}

size_t copy_dns_name(const unsigned char *reader, const unsigned char *buffer,
                     std::string &out_name) {
  /*
   * Copies name starting at reader into out_name.
   * As parts of the name may be defined elsewhere in the buffer, also needs a
   * pointer to the buffer start. Returns the offset after reading to the end of
   * name or first jump.
   */
  bool have_jumped{false};
  size_t advance_count{1};

  while (*reader != '\0') {
    if (*reader >= 192)  // need to jump
    {
      unsigned int offset;
      offset = (*reader) * 256 + *(reader + 1) - 0xc000;
      reader = buffer + offset;
      have_jumped = true;
      advance_count += 1;
    } else {
      out_name.push_back(*(reader++));
      if (!have_jumped) {
        advance_count += 1;
      }
    }
  }

  for (int walk{0}, next_jump{}; walk < out_name.length();) {
    next_jump = static_cast<int>(out_name[walk]);
    out_name[walk] = '.';
    walk += (next_jump + 1);
  }

  // Strip leading dot
  out_name.erase(0, 1);

  return advance_count;
}

// LCOV_EXCL_START
std::ostream &operator<<(std::ostream &ostream, const DnsHeader &dns_header) {
  ostream << "questions: " << ntohs(dns_header.question_count) << '\n'
          << "answers: " << ntohs(dns_header.answer_count) << '\n'
          << "authority records: " << ntohs(dns_header.authority_count) << '\n'
          << "additional records: " << ntohs(dns_header.additional_record_count)
          << '\n';
  return ostream;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
std::ostream &operator<<(std::ostream &ostream,
                         const ResourceRecordHeader &record_header) {
  ostream << "class: " << ntohs(record_header.record_class) << '\n'
          << "type: " << ntohs(record_header.type) << '\n'
          << "ttl: " << ntohl(record_header.ttl) << '\n'
          << "length: " << ntohs(record_header.resource_data_length) << '\n';
}
// LCOV_EXCL_STOP
