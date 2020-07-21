#include <crypto/Curve25519.h>
#include "gtest/gtest.h"

TEST(X25519Test, first_RFC7748_test_vector) {
  FieldElement scalar{
      "a5 46 e3 6b f0 52 7c 9d 3b 16 15 4b 82 46 5e dd 62 14 4c 0a c1 fc 5a 18 "
      "50 6a 22 44 ba 44 9a c4"};
  FieldElement u_coordinate{
      "e6 db 68 67 58 30 30 db 35 94 c1 a4 24 b1 5f 7c 72 66 24 ec 26 b3 35 3b "
      "10 a9 03 a6 d0 ab 1c 4c"};

  FieldElement expected_output{
      "c3 da 55 37 9d e9 c6 90 8e 94 ea 4d f2 8d 08 4f 32 ec cf 03 49 1c 71 f7 "
      "54 b4 07 55 77 a2 85 52"};

  EXPECT_EQ(X25519(scalar, u_coordinate), expected_output);
}

TEST(X25519Test, second_RFC7748_test_vector) {
  FieldElement scalar{
      "4b 66 e9 d4 d1 b4 67 3c 5a d2 26 91 95 7d 6a f5 c1 1b 64 21 e0 ea 01 d4 "
      "2c a4 16 9e 79 18 ba 0d"};
  FieldElement u_coordinate{
      "e5 21 0f 12 78 68 11 d3 f4 b7 95 9d 05 38 ae 2c 31 db e7 10 6f c0 3c 3e "
      "fc 4c d5 49 c7 15 a4 93"};

  FieldElement expected_output{
      "95 cb de 94 76 e8 90 7d 7a ad e4 5c b4 b8 73 f8 8b 59 5a 68 79 9f a1 52 "
      "e6 f8 f7 64 7a ac 79 57"};

  EXPECT_EQ(X25519(scalar, u_coordinate), expected_output);
}

TEST(X25519Test, RFC7748_iteration_test_1_iteration) {
  FieldElement scalar{
      "09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00"};
  FieldElement u_coordinate{
      "09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00"};

  FieldElement expected_output{
      "42 2c 8e 7a 62 27 d7 bc a1 35 0b 3e 2b b7 27 9f 78 97 b8 7b b6 85 4b 78 "
      "3c 60 e8 03 11 ae 30 79"};
  EXPECT_EQ(X25519(scalar, u_coordinate), expected_output);
}

// takes about half an hour to run
TEST(X25519Test, DISABLED_RFC7748_iteration_test_1000_iterations) {
  FieldElement scalar{
      "09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00"};
  FieldElement u_coordinate{
      "09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00"};

  FieldElement last_result{0};
  for (int i{0}; i < 1000; ++i) {
    last_result = X25519(scalar, u_coordinate);

    u_coordinate = scalar;
    scalar = last_result;
  }

  FieldElement expected_output{
      "68 4c f5 9b a8 33 09 55 28 00 ef 56 6f 2f 4d 3c 1c 38 87 c4 93 60 e3 87 "
      "5f 2e b9 4d 99 53 2c 51"};
  EXPECT_EQ(last_result, expected_output);
}

// not at all feasible to run...
TEST(X25519Test, DISABLED_RFC7748_iteration_test_1000000_iterations) {
  FieldElement scalar{
      "09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00"};
  FieldElement u_coordinate{
      "09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
      "00 00 00 00 00 00 00 00"};

  FieldElement last_result{0};
  for (int i{0}; i < 1000000; ++i) {
    last_result = X25519(scalar, u_coordinate);

    u_coordinate = scalar;
    scalar = last_result;
  }

  FieldElement expected_output{
      "7c 39 11 e0 ab 25 86 fd 86 44 97 29 7e 57 5e 6f 3b c6 01 c0 88 3c 30 df "
      "5f 4d d2 d2 4f 66 54 24"};
  EXPECT_EQ(last_result, expected_output);
}

TEST(X25519Test, RFC7748_Diffie_Hellman_test_vector) {
  FieldElement base_point{9};

  FieldElement alice_private_key{
      "77 07 6d 0a 73 18 a5 7d 3c 16 c1 72 51 b2 66 45 df 4c 2f 87 eb c0 99 2a "
      "b1 77 fb a5 1d b9 2c 2a"};
  FieldElement alice_public_key{X25519(alice_private_key, base_point)};
  ASSERT_EQ(alice_public_key.to_string(),
            "85 20 f0 09 89 30 a7 54 74 8b 7d dc b4 3e f7 5a 0d bf 3a 0d 26 38 "
            "1a f4 eb a4 a9 8e aa 9b 4e 6a");

  FieldElement bob_private_key{
      "5d ab 08 7e 62 4a 8a 4b 79 e1 7f 8b 83 80 0e e6 6f 3b b1 29 26 18 b6 fd "
      "1c 2f 8b 27 ff 88 e0 eb"};
  FieldElement bob_public_key{X25519(bob_private_key, base_point)};
  ASSERT_EQ(bob_public_key.to_string(),
            "de 9e db 7d 7b 7d c1 b4 d3 5b 61 c2 ec e4 35 37 3f 83 43 c8 5b 78 "
            "67 4d ad fc 7e 14 6f 88 2b 4f");

  FieldElement alice_shared_secret{X25519(alice_private_key, bob_public_key)};
  FieldElement bob_shared_secret{X25519(bob_private_key, alice_public_key)};
  FieldElement expected_shared_secret{
      "4a 5d 9d 5b a4 ce 2d e1 72 8e 3b f4 80 35 0f 25 e0 7e 21 c9 47 d1 9e 33 "
      "76 f0 9b 3c 1e 16 17 42"};
  EXPECT_EQ(alice_shared_secret, expected_shared_secret);
  EXPECT_EQ(bob_shared_secret, expected_shared_secret);
}
