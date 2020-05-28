#include "dns/DNSQuerier.h"
#include "dns/dns.h"
#include "gtest/gtest.h"

TEST(DNSQuerierTest, simple_run) {
  DNSQuerier querier;

  std::string resolution{querier.query("borys.dk.")};
  std::string expected_resultion{"212.47.229.133"};

  EXPECT_EQ(resolution, expected_resultion);
}
