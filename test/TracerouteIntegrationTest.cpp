#include <future>
#include "gtest/gtest.h"
#include "traceroute/Traceroute.h"

TEST(TracerouteIntegrationTest, can_create_socket) { Traceroute tracer; }

TEST(TracerouteIntegrationTest, simple_run) {
  Traceroute tracer;
  int max_hops{10};
  std::string destination{"8.8.8.8"};

  auto read_future = std::async(&Traceroute::read_until_empty, &tracer, 1);
  for (int i{1}; i <= max_hops; ++i) {
    tracer.ping(destination, i);
  }
  read_future.wait();
}
