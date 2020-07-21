#ifndef NETTOOLS_SRC_CRYPTO_CURVE25519_H_
#define NETTOOLS_SRC_CRYPTO_CURVE25519_H_

#include "FieldElement.h"
/*
 * =================================================================
 *                             Caution:
 * THIS IMPLEMENTATION IS LIKELY VULNERABLE TO SIDE-CHANNEL ATTACKS!
 * =================================================================
 *
 * see https://tools.ietf.org/html/rfc7748
 * Montgomery curve y^2 = x^3 + A*x^2 + x
 * p = 2^255 - 19
 * A = 486662
 *
 */

using Coordinates = std::pair<FieldElement, FieldElement>;
Coordinates double_point(const Coordinates &coordinates);
Coordinates add_point_distinct(const Coordinates &point,
                               const Coordinates &other_point);

FieldElement X25519(const FieldElement &scalar,
                    const FieldElement &start_coordinate);
#endif  // NETTOOLS_SRC_CRYPTO_CURVE25519_H_
