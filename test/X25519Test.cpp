#include <crypto/Curve25519.h>
#include "gtest/gtest.h"

TEST(X25519Test, first_RFC_test_vector) {
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

TEST(X25519Test, second_RFC_test_vector) {
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
