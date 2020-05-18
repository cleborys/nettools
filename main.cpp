#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <future>
#include <iostream>

#include "Traceroute.h"

int main() {
  Traceroute tracer;
  std::string destination{"172.217.19.78"};

  auto read_future = std::async(&Traceroute::read_until_empty, &tracer, 10);
  for (int i{1}; i < 15; ++i) {
    tracer.ping("172.217.19.78", i);
  }
  read_future.wait();

  tracer.print_records();
  return 0;
}
