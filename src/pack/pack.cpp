/**********************************************************************
    CIP_SIS (Confidential Integer Processing)
    Copyright (C) 2022  Jayamine Alupotha

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "pack.h"


/**
 * Pack a polynomial
 * @param bytes - ouput byte array
 * @param a - input polynomial
 */
void pack_custom_poly_internal(uint8_t *bytes, poly *a, const int bound_byte) {
    int b, i, j, byte;
    zint_t coef;

    memset(bytes, 0x0, bound_byte * (256/8));

    for (b = 0; b < (N/8); b++) { // blocks
        byte = b * bound_byte;
        for (j = 0; j < 8; j++) {  // each byte while 0th byte is the sign byte
            coef = a->coef[b * 8 + j];
            if (coef < 0) {
                bytes[byte] |= (1 << j);
                coef = -coef;
            }
            for (i = 1; i < bound_byte; i++) {
                bytes[byte + i] |= (uint8_t) (((coef >> (i - 1)) & 1) << j);
            }
        }
    }
}

/**
 * Convert a byte array to a polynomial in [-Q2, Q2]
 * @param a - output polynomial
 * @param bytes - input byte array
 */
void unpack_custom_poly_internal(poly *a, const uint8_t *bytes, const int bound_byte) {
    unsigned int b, i, j, byte;

    for (b = 0; b < (N/8); b++) { // blocks
        byte = b * bound_byte;
        for (j = 0; j < 8; j++) {  // each byte while 0th byte is the sign byte
            a->coef[b * 8 + j] = 0;
            for (i = 1; i < bound_byte; i++) {
                a->coef[b * 8 + j] |= (((zint_t)(bytes[byte + i] >> j) & 1) << (i - 1));
            }
            if (((bytes[byte] & (1 << j)) >> j) == 1) {
                a->coef[b * 8 + j] = -a->coef[b * 8 + j];
            }
        }
    }
}

/**
 * Pack a polynomial
 * @param bytes - ouput byte array
 * @param a - input polynomial
 */
void pack_custom_poly(uint8_t *bytes, poly *a) {
    pack_custom_poly_internal(bytes, a, K1);
}

/**
 * Convert a byte array to a polynomial in [-Q2, Q2]
 * @param a - output polynomial
 * @param bytes - input byte array
 */
void unpack_custom_poly(poly *a, const uint8_t *bytes) {
    unpack_custom_poly_internal(a, bytes, K1);
}

/**
 * Pack a polynomial vector to a byte array
 * @param bytes - output byte array
 * @param u - input polynomial vector
 */
void pack_poly_ring(uint8_t bytes[n * u_BYTES], poly_n *u) {
    unsigned int i;
    for (i = 0; i < n; i++) {
        pack_custom_poly(bytes + u_BYTES*i, &u->vec[i]);
    }
}

/**
 * Convert a byte array to a polynomial vector
 * @param u - output polynomial vector
 * @param bytes - input byte array
 */
void unpack_poly_ring(poly_n *u, const uint8_t bytes[n * u_BYTES]) {
    unsigned int i;
    for (i = 0; i < n; i++) {
        unpack_custom_poly(&u->vec[i], bytes + u_BYTES*i);
    }
}

/**
 * Pack a polynomial vector to a byte array
 * @param bytes - output byte array
 * @param u - input polynomial vector
 * @param p - the number of higher bits
 */
void pack_poly_ring_custom(uint8_t bytes[], poly_n *u, int p) {
    unsigned int i;
    for (i = 0; i < n; i++) {
        pack_custom_poly_internal(bytes + (p * N/8)*i, &u->vec[i], p);
    }
}

/**
 * Convert a byte array to a polynomial vector
 * @param u - output polynomial vector
 * @param bytes - input byte array
 * @param p - the number of higher bits
 */
void unpack_poly_ring_custom(poly_n *u, const uint8_t bytes[], int p) {
    unsigned int i;
    for (i = 0; i < n; i++) {
        unpack_custom_poly_internal(&u->vec[i], bytes + (p * N/8)*i, p);
    }
}

/**
 * Pack a polynomial vector to a byte array
 * @param bytes - output byte array
 * @param u - input polynomial vector
 * @param p - the number of higher bits
 */
void pack_poly_com_custom(uint8_t bytes[], poly_n *u) {
    poly tmp;
    unsigned int i;
    for (i = 0; i < n; i++) {
        poly_highbits(&tmp, &u->vec[i], u_ERROR);
        pack_custom_poly_internal(bytes + ((K1 - u_ERROR) * N/8)*i, &tmp, (K1 - u_ERROR));
    }
}

/**
 * Convert a byte array to a polynomial vector
 * @param u - output polynomial vector
 * @param bytes - input byte array
 * @param p - the number of higher bits
 */
void unpack_poly_com_custom(poly_n *u, const uint8_t bytes[]) {
    unsigned int i;
    for (i = 0; i < n; i++) {
        unpack_custom_poly_internal(&u->vec[i], bytes + ((K1 - u_ERROR) * N/8)*i, (K1 - u_ERROR));
        poly_roundup(&u->vec[i], &u->vec[i], u_ERROR);
    }
}

/**
 * Pack the coefficients of a polynomial vector to a byte array. Note that
 * those coefficients must be in [-ALPHA, ALPHA]
 * @param bytes - output bytes
 * @param s - input polynomial
 */
#define BLOCK (((ALPHA_BITS + 1) * N)/8)
void pack_poly_z(uint8_t bytes[z_BYTES], poly s[L]) {
    int l;

    for (l = 0; l < L; l++) {
        pack_custom_poly_internal(bytes + BLOCK * l, &s[l], (ALPHA_BITS + 1));
    }
}

/**
 * Convert a byte array to the coefficients of a polynomial. Note that
 * those coefficients are in [-ALPHA, ALPHA]
 * @param s - output polynomial
 * @param bytes - input bytes
 */
void unpack_poly_z(poly s[L], const uint8_t bytes[z_BYTES]) {
    int l;

    for (l = 0; l < L; l++) {
        unpack_custom_poly_internal(&s[l], bytes + BLOCK * l, (ALPHA_BITS + 1));
    }
}

/**
 * Pack all coefficients of a polynomial vector to a byte array. Note that
 * Those coefficients must be in [-TAU2, TAU2].
 * @param bytes - output byte array
 * @param s - input polynomial vector
 */
#define SINGLE_R_BYTES  (N * (TAU2_BITS + 1)/8)
void pack_poly_m_R(uint8_t bytes[r2_BYTES],  poly s[mk]) {
    int l;

    for (l = 0; l < (mk); l++) {
        pack_custom_poly_internal(bytes + SINGLE_R_BYTES * l, &s[l], (TAU2_BITS + 1));
    }
}

/**
 * Convert a byte array to coefficients of a polynomial vector. Those coefficients will be in [-TAU2, TAU2]
 * @param s - output polynomial vector
 * @param bytes - input byte array
 */
void unpack_poly_m_R(poly s[mk], const uint8_t bytes[R_BYTES]) {
    int l;

    for (l = 0; l < (mk); l++) {
        unpack_custom_poly_internal(&s[l], bytes + SINGLE_R_BYTES * l, (TAU2_BITS + 1));
    }
}

/**
 * Pack all coefficients of a polynomial vector of size m - D to a byte array. Note that
 * Those coefficients must be in [-TAU3, TAU3].
 * @param bytes - output byte array
 * @param s - input polynomial vector
 */
#define SINGLE_sig_BYTES (N * (TAU3_BITS + 1 + 1)/8)
void pack_poly_sig(uint8_t bytes[sig_BYTES], poly s[m - D]) {
    int l;

    for (l = 0; l < (m - D); l++) {
        pack_custom_poly_internal(bytes + SINGLE_sig_BYTES * l, &s[l], (TAU3_BITS + 1));
    }
}

/**
 * Convert a byte array to coefficients of a polynomial vector. Those coefficients will be in [-TAU2, TAU2]
 * @param s - output polynomial vector
 * @param bytes - input byte array
 */
void unpack_poly_sig(poly s[m - D], const uint8_t bytes[sig_BYTES]) {
    int l;

    for (l = 0; l < (m - D); l++) {
        unpack_custom_poly_internal(&s[l], bytes + SINGLE_sig_BYTES * l, (TAU3_BITS + 1));
    }
}


