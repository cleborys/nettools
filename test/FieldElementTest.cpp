#include <crypto/FieldElement.h>
#include "gtest/gtest.h"

TEST(FieldElementTest, create_from_uint32) {
  std::uint32_t value{0x12ab34cd};

  FieldElement field_element{value};

  EXPECT_EQ(field_element.to_string().substr(0, 20), "cd 34 ab 12 00 00 00");
}

TEST(FieldElementTest, operator_xor) {
  std::uint32_t x{0x12ab34};
  std::uint32_t y{0x84c39f};

  EXPECT_EQ(FieldElement(x) ^ FieldElement(y), FieldElement(x ^ y));
}

TEST(FieldElementTest, operator_and) {
  std::uint32_t x{0x12ab34};
  std::uint32_t y{0x84c39f};

  EXPECT_EQ(FieldElement(x) & FieldElement(y), FieldElement(x & y));
}

TEST(FieldElementTest, small_addition) {
  std::uint32_t x{0x12ab34};
  std::uint32_t y{0x84c39f};
  std::uint32_t result{x + y};

  FieldElement field_x{x};
  FieldElement field_y{y};
  FieldElement field_result{result};

  EXPECT_EQ(field_x + field_y, field_result);
}

TEST(FieldElementTest, shift_operator) {
  for (int i{0}; i < 32; ++i) {
    EXPECT_EQ(FieldElement(1) << i, FieldElement(1 << i));
  }

  for (int i{0}; i < 16; ++i) {
    EXPECT_EQ(FieldElement(0b100111001) << i, FieldElement(0b100111001 << i));
  }
}

TEST(FieldElementTest, right_shift_operator) {
  for (int i{0}; i < 32; ++i) {
    EXPECT_EQ(FieldElement(0xabcd) >> i, FieldElement(0xabcd >> i));
  }

  FieldElement some_element{0xd9e8};
  for (int i{0}; i < 200; ++i) {
    EXPECT_EQ(some_element, (some_element << i) >> i);
  }
}

TEST(FieldElementTest, normalize_p_plus_nineteen) {
  FieldElement p_plus_nineteen{FieldElement(1) << 255};
  EXPECT_EQ(p_plus_nineteen.normalize(), FieldElement(19));
}

TEST(FieldElementTest, normalize_some_large_element) {
  FieldElement some_large_element{(FieldElement(1) << 255) +
                                  FieldElement{0x1353} +
                                  (FieldElement{0x753} << 100)};
  FieldElement same_in_normalized{FieldElement{19} + FieldElement{0x1353} +
                                  (FieldElement{0x753} << 100)};
  EXPECT_EQ(some_large_element.normalize(), same_in_normalized);
}

TEST(FieldElementTest, normalize_leaves_small_elements_alone) {
  FieldElement reasonably_small{FieldElement{19} + FieldElement{0x1353} +
                                (FieldElement{0x753} << 100)};
  EXPECT_EQ(reasonably_small.normalize(), reasonably_small);
}

TEST(FieldElementTest, subtraction) {
  FieldElement two_to_the_255{FieldElement{1} << 255};
  EXPECT_EQ(two_to_the_255 - FieldElement{19}, FieldElement{0});
}

TEST(FieldElementTest, small_multiplication) {
  int a{3749};
  int b{1927};
  EXPECT_EQ(FieldElement(a) * FieldElement(b), FieldElement(a * b));
}

TEST(FieldElementTest, large_powers) {
  FieldElement shifted{1};
  FieldElement multiplied{1};

  for (int i{0}; i < 500; ++i) {
    EXPECT_EQ(multiplied, shifted);
    multiplied = multiplied * FieldElement{2};
    shifted = (shifted << 1).normalize();
  }
}

TEST(FieldElementTest, bit_at) {
  int some_integer{0b101010100110100101010101010101};
  FieldElement element{static_cast<uint32_t>(some_integer)};
  for (int i{0}; i < 32; ++i) {
    EXPECT_EQ(element.bit_at(i), bool(some_integer & (1 << i)));
  }
}

TEST(FieldElementTest, power) {
  FieldElement three{3};
  FieldElement walk{1};

  for (int i{0}; i < 15; ++i) {
    EXPECT_EQ(three.pow(FieldElement(i)), walk);
    walk = walk * three;
  }
}
TEST(FieldElementTest, inverse) {
  FieldElement one{1};
  EXPECT_EQ(one.inverse(), one);

  FieldElement element{0x23404af};

  EXPECT_EQ(element.inverse() * element, one);
}

TEST(FieldElementTest, from_hex_byte_string) {
  std::string hex_bytes{
      "a5 46 e3 6b f0 52 7c 9d 3b 16 15 4b 82 46 5e dd 62 14 4c 0a c1 fc 5a 18 "
      "50 6a 22 44 ba 44 9a c4"};

  FieldElement element{hex_bytes};

  EXPECT_EQ(element.to_string(), hex_bytes);
}
