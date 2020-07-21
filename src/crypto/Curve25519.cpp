#include "Curve25519.h"

const FieldElement CURVE_A{486662};
const FieldElement CURVE_A24{121665};  // (CURVE_A - 2) / 4;

/*
 * Arithmetic following RFC 7748
 *
 */

void conditional_swap(bool swap, FieldElement &element,
                      FieldElement &other_element) {
  // slightly convoluted to make the runtime independent of swap
  // see page 9 of RFC 7748
  std::array<FieldElement, 2> swap_mask = {
      FieldElement(0),
      FieldElement{"ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                   "ff ff ff ff ff ff ff ff ff ff ff ff ff"},
  };
  FieldElement xor_element{element ^ other_element};
  FieldElement swap_or_not{swap_mask[swap] & xor_element};
  element = swap_or_not ^ element;
  other_element = swap_or_not ^ other_element;
}

FieldElement X25519(const FieldElement &scalar,
                    const FieldElement &start_coordinate) {
  // For reference see page 8 of RFC 7748

  FieldElement prepared_scalar{scalar.prepare_scalar()};
  FieldElement x_1{start_coordinate.mask_most_significant_bit().normalize()};
  FieldElement x_2{1};
  FieldElement z_2{0};
  FieldElement x_3{x_1};
  FieldElement z_3{1};
  bool swap{false};

  bool k_t{};
  for (size_t i{0}; i < FIELD_SIZE * 8; ++i) {
    size_t bit_number_counted_from_right{FIELD_SIZE * 8 - 1 - i};
    k_t = prepared_scalar.bit_at(bit_number_counted_from_right);

    swap = (swap != k_t);  // XOR for bool
    conditional_swap(swap, x_2, x_3);
    conditional_swap(swap, z_2, z_3);
    swap = k_t;

    FieldElement a24{(CURVE_A - FieldElement(2)) * FieldElement(4).inverse()};
    FieldElement A{x_2 + z_2};
    FieldElement AA{A * A};
    FieldElement B{x_2 - z_2};
    FieldElement BB{B * B};
    FieldElement E{AA - BB};
    FieldElement C{x_3 + z_3};
    FieldElement D{x_3 - z_3};
    FieldElement DA{D * A};
    FieldElement CB{C * B};
    x_3 = (DA + CB) * (DA + CB);
    z_3 = x_1 * (DA - CB) * (DA - CB);
    x_2 = AA * BB;
    z_2 = E * (AA + CURVE_A24 * E);
  }

  conditional_swap(swap, x_2, x_3);
  conditional_swap(swap, z_2, z_3);

  return x_2 * z_2.inverse();
}

/*
 * Arithmetic following https://cr.yp.to/ecdh/curve25519-20060209.pdf
 * See top of page 20 ("The elliptic curve")
 *
 * Addition of (x,y) + (x,y) gives
 * result_x = (x^2 - 1)^2 / 4y^2
 * result_y = (3x^2 + 2Ax + 1)/2y * (x - result_x) - y
 *
 * Addition of (x,y) + (x',y')
 * where x \neq \x' is given by
 * \lambda = (y'-y)/(x'-x)
 * result_x = \lambda^2 - A - x - x'
 * result_y = \lambda (x-result_x) - y
 *
 */

Coordinates double_point(const Coordinates &coordinates) {
  const FieldElement &x{coordinates.first};
  const FieldElement &y{coordinates.second};

  FieldElement result_x{(x * x - 1) * (x * x - 1) *
                        (FieldElement(4) * y * y).inverse()};
  FieldElement result_y{(FieldElement(3) * x * x +
                         FieldElement(2) * CURVE_A * x + FieldElement(1)) *
                            (FieldElement(2) * y).inverse() * (x - result_x) -
                        y};

  return Coordinates(result_x, result_y);
}

Coordinates add_point_distinct(const Coordinates &point,
                               const Coordinates &other_point) {
  const FieldElement &x{point.first};
  const FieldElement &y{point.second};
  const FieldElement &x_prime{other_point.first};
  const FieldElement &y_prime{other_point.second};

  FieldElement lambda{(y_prime - y) * (x_prime - x).inverse()};

  FieldElement result_x{lambda * lambda - CURVE_A - x - x_prime};
  FieldElement result_y{lambda * (x - result_x) - y};

  return Coordinates(result_x, result_y);
}
