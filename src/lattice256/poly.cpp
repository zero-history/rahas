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

#include "poly.h"

/**
 * Reduce 128bit number to the range [-2^K1, 2^K1]
 *
 * @param a - the number
 * @return a number in [-2^K1, 2^K1]
 */
zint_t reduceK1(const zint_t a) {
    zint_t t;
    t = (a + ((zint_t)1 << K11)) >> K1;
    t = (zint_t)((zint2_t) a - (zint2_t)t * (zint2_t)Q); // bring t to [-2^50, 2^50]

    return t;
}

/**
 * Reduce 128bit number to the range [-(Q-1)/2, (Q-1)/2]
 *
 * @param a - the number
 * @return a number in [-(Q-1)/2, (Q-1)/2]
 */
zint_t reduceK1_exact(zint_t a) {
    zint_t t;
    t = (a + ((zint_t)1 << K11)) >> K1;
    t = (zint_t)((zint2_t) a - (zint2_t)t * (zint2_t)Q); // bring t to [-2^50, 2^50]

    if (t > Q2) t = -Q + t;
    if (t < -Q2) t = Q + t;

    return t;
}

/**
 * Get a1 such that a = a1 * 2^p + a2 and |a2| < 2^p
 * @param a - input
 * @param p - the number of lower bits
 * @return
 */


zint_t highbits(zint_t a, unsigned int p) {
    //int64_t t =  ((((int64_t)(a & (((int64_t)1) << (p - 1)) - 1)) << (64 - K11))>> (p + 64 - K11));
    if (a == (Q - 1) >> 1) a = 0;
    zint_t t =  (a & ((((zint_t) 1 << (K1 - p)) - 1) << (p - 1))) >> p;
    return t;
}

/**
 * Get a1 * 2^p
 * @param a - input
 * @param p - the number of lower bits
 * @return
 */
zint_t roundup(zint_t a, unsigned int p) {
    return reduceK1(a << p);
}

zint_t lowbit_clean(zint_t a, unsigned int p) {
    //int64_t t =  ((((int64_t)(a & (((int64_t)1) << (p - 1)) - 1)) << (64 - K11))>> (p + 64 - K11));
    if (a == (Q - 1) >> 1) a = 0;
    zint_t t =  (a & ((((zint_t) 1 << (K1 - p)) - 1) << (p - 1)));
    return t;
}

/**
 * Initiate a polynomial from a vector
 * @param vec - input
 * @return a polynomial
 */
poly poly_from_vec(const zint_t vec[N]) {
    int i;
    poly a;
    for (i = 0; i < N; i++) {
        a.coef[i] = vec[i];
    }
    return a;
}

/**
 * Set b = a
 * @param b - output polynomial
 * @param a - input polynomial
 */
void poly_set(poly *b, poly *a) {
    unsigned int j;
    for (j = 0; j < N; j++)
        b->coef[j] = (zint_t) a->coef[j];
}

/**
 * Compare two polynomials
 * @param s1 - input polynomial
 * @param s2 - input polynomial
 * @return 0 - equal and -1 - otherwise
 */
int poly_compare(const poly *s1, const poly *s2) {
    for (int i = 0; i < N; i++) {
        if (s1->coef[i] != s2->coef[i]){
#if defined(ENABLE_DEBUG_MODE)
            printf("%d %ld %ld\n", i, s1->coef[i], s2->coef[i]);
#endif
            return -1;
        }
    }
    return 0;
}

/**
 * Compare (K1 - p) number of high bits of each element
 * @param s1 - input polynomial
 * @param s2 - input polynomial
 * @param p - the number of lower bits
 */
int poly_highbits_compare(const poly *s1, const poly *s2, unsigned int p) {
    for (int i = 0; i < N; i++) {
        if (highbits(s1->coef[i], p) != highbits(s2->coef[i], p)){
#if defined(ENABLE_DEBUG_MODE)
            printf("%d %ld %ld\n", i, (s1->coef[i] & filter), (s2->coef[i] & filter));
#endif
            return -1;
        }
    }
    return 0;
}

/**
 * Set [start:end] to zero (except "end")
 * @param a - input polynomial
 * @param start - starting index
 * @param end - ending index
 * @return
 */
void poly_set_zero(poly *a, unsigned int start, unsigned int end) {
    unsigned int i;
    for (i = start; i < end; i++) {
        a->coef[i] = (zint_t)0;
    }
}

/**
 * Set a.coef[j] = val and other coefficients to zero
 * @param a
 * @param val
 * @param j
 */
void poly_rot(poly *a, const zint_t val, unsigned int j) {
    unsigned int i;
    for (i = 0; i < N; i++) {
        a->coef[i] = (zint_t) 0;
    }
    a->coef[j] = val;
}

/**
 * Change range to [0, Q-1]
 * @param a
 * @return
 */
static zint_t addq(zint_t a) {
    a += (a >> K11) & Q;
    return a;
}


/**
 * Get high bits of each coefficient
 * @param a - out polynomial
 * @param b - input polynomial
 * @param p - the number of lower bits
 */
void poly_highbits(poly *a, poly *b, unsigned int p) {
    unsigned int i;
    for (i = 0; i < N; i++) {
        a->coef[i] = highbits(b->coef[i], p);
    }
}

/**
 * Get the roundup of each coefficient such that a*2^p
 * @param a - out polynomial
 * @param b - input polynomial
 * @param p - the number of lower bits
 */
void poly_roundup(poly *a, poly *b, unsigned int p) {
    unsigned int i;
    for (i = 0; i < N; i++) {
        a->coef[i] = roundup(b->coef[i], p);
    }
}

/**
 * Remove low bits of each coefficient
 * @param a - out polynomial
 * @param b - input polynomial
 * @param p - the number of lower bits
 */
void poly_lowbit_clean(poly *a, poly *b, unsigned int p) {
    unsigned int i;
    for (i = 0; i < N; i++) {
        a->coef[i] = lowbit_clean(b->coef[i], p);
    }
}

/**
 * Reduce all coefficients of a polynomial to [-2^K1, 2^K1]
 * @param a - the polynomial
 */
void poly_reduce(poly *a) {
    unsigned int i;
    for (i = 0; i < N; i++) {
        a->coef[i] = reduceK1(a->coef[i]);
    }
}

/**
 * Reduce all coefficients of a polynomial to [-Q, Q]
 * @param a - the polynomial
 */
void poly_reduce_exact(poly *a) {
    unsigned int i;
    for (i = 0; i < N; i++) {
        a->coef[i] = reduceK1_exact(a->coef[i]);
    }
}

/**
 * Change coefficient range of a polynomial to [0, Q-1]
 * @param a
 */
void poly_addq(poly *a) {
    unsigned int i;
    for (i = 0; i < N; ++i)
        a->coef[i] = addq(a->coef[i]);
}

/**
 * Add two polynomials without any modular reductions
 * @param c - input polynomial
 * @param a - input polynomial
 * @param b - a + b
 */
void poly_add(poly *c, poly *a, poly *b) {
    unsigned int i;
    for (i = 0; i < N; ++i)
        c->coef[i] = (zint_t) (a->coef[i] + b->coef[i]);
}

/**
 * Subtract two polynomials without any modular reductions
 * @param c - input polynomial
 * @param a - input polynomial
 * @param b - a - b
 */
void poly_sub(poly *c, poly *a, poly *b) {
    unsigned int i;
    for (i = 0; i < N; ++i)
        c->coef[i] = (zint_t) (a->coef[i] - b->coef[i]);
}

/**
 * Multiply by 2^l
 * @param a
 */
void poly_shift_l(poly *a, unsigned int l) {
    unsigned int i;
    for (i = 0; i < N; ++i)
        a->coef[i] <<= l;
}

/**
 * Apply NTT forward transformation to a polynomial
 * @param a - polynomial
 */
void poly_ntt(poly *a) {
    ntt(a->coef);
}

/**
 * Apply NTT inverse transformation to a polynomial
 * and outputs a polynomial multiplied by the Montgomery factor.
 * @param a - polynomial
 */
void poly_inv_ntt_to_mont(poly *a) {
    invntt_tomont(a->coef);
}

/**
 * Apply NTT inverse transformation to a polynomial
 * @param a - polynomial
 */
void poly_inv_ntt(poly *a) {
    invntt(a->coef);
}

/**
 * Point-wise multiplication of polynomials in NTT domain
 * @param a - input
 * @param b - input
 * @param c - a*b (elementwise)
 */
void poly_pointwise_montgomery(poly *c, poly *a, poly *b) {
    unsigned int i;

    for(i = 0; i < N; i++)
        c->coef[i] = montgomery_reduce((zint2_t)((zint2_t)a->coef[i] * (zint2_t)b->coef[i]));

}

/**
 * c = a * b in Z[X]/[X^N + 1] where b.coef[i] = value and all other coefficients are zero.
 * @param c - output
 * @param a - input
 * @param i - index
 * @param value - ith coefficient
 */
void poly_easy_mul(poly *c, poly *a, unsigned int i, zint_t value) {
    unsigned int j;
    poly tmp; // Should not directly set c because a can be the same pointer as c.
    for (j = 0; j < (N - i); j++) {
        tmp.coef[j + i] = value * a->coef[j];
    }
    for (j = (N - i); j < N; j++) {
        tmp.coef[j - N + i] = -(value * a->coef[j]);
    }
    poly_set(c, &tmp);
}

/**
 * Check infinity norm
 * @param a - polynomial
 * @param B - norm
 * @return 0 if B <= GAMMA2 and |a|_infinity < B; -1 otherwise
 */
int poly_chknorm(const poly *a, zint_t B) {
    unsigned int i;
    zint_t t;

    if (B > GAMMA2)
        return -1;

    for (i = 0; i < N; ++i) {
        // Absolute value
        t = a->coef[i] >> K11;
        t = a->coef[i] - (t & 2 * a->coef[i]);

        if (t >= B) {
            //cout << i << " " << a->coef[i] << "\n";
            return -1;
        }
    }

    return 0;
}


/**
 * Check infinity norm for a customized range
 * @param a  - polynomial
 * @param B  - norm
 * @param start - start point
 * @param end - end point
 * @return 0 if B <= GAMMA2 and |a[start:end]|_infinity < B; -1 otherwise
 */
int poly_chknorm_custom(const poly *a, zint_t B, int start, int end) {
    unsigned int i;
    zint_t t;

    if (B > GAMMA2 || start < 0 || end > N)
        return -1;

    for (i = start; i < end; ++i) {
        // Absolute value
        t = a->coef[i] >> K11;
        t = a->coef[i] - (t & 2 * a->coef[i]);

        if (t >= B) {
            return -1;
        }
    }

    return 0;
}

/**
 * Generate challenge polynomial with CH number of +1/-1
 * @param x  - output polynomial
 * @param seed - input hash bytes
 */
void poly_challenge(poly *x, const uint8_t seed[SEED_BYTES]) {
    unsigned int i, b, pos;
    zuint_t signs;
    uint8_t buf[SHAKE256_RATE];
    keccak_state state;

    shake256_init(&state);
    shake256_absorb(&state, seed, SEED_BYTES);
    shake256_finalize(&state);
    shake256_squeezeblocks(buf, 1, &state);

    signs = 0;
    for(i = 0; i < 8; ++i)
        signs |= (zuint_t)buf[i] << 8*i;
    pos = 8;

    for(i = 0; i < N; ++i)
        x->coef[i] = 0;
    for(i = N - BETA; i < N; ++i) {
        do {
            if(pos >= SHAKE256_RATE) {
                shake256_squeezeblocks(buf, 1, &state);
                pos = 0;
            }

            b = buf[pos++];
        } while(b > i);

        x->coef[i] = x->coef[b];
        x->coef[b] = (zint_t)1 - 2*(signs & 1);
        signs >>= 1;
    }
}


