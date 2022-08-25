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

#include "algo.h"

/**
 * Add commitments (u0 = u1 + u2), values and keys
 * @param ctx - context object
 * @param d - number of values
 * @param u0 - out commitment
 * @param v0 - out values
 * @param k0 - out key
 * @param u1 - in commitment 1
 * @param v1 - in values
 * @param k1 - in keys
 * @param u2 - in commitment 2
 * @param v2 - in values
 * @param k2 - in keys
 */
void add_gen(context_t *ctx, uint8_t d,
             com_t *u0, zint_t *v0, mask_t *k0,
             com_t *u1, zint_t *v1, mask_t *k1,
             com_t *u2, zint_t *v2, mask_t *k2) {

    int j;
    poly_n_add(u0, u1, u2);
    poly_n_reduce(u0);
    for (j = 0; j < d; j++)
        v0[j] = v1[j] + v2[j];

    for (j = 0; j < mk; j++)
        poly_add(&k0->k[j], &k1->k[j], &k2->k[j]);
}

/**
 * Verifies whether u0 ?= u1 + u2
 * @param ctx - context
 * @param u0 - output to be verified
 * @param u1 - input
 * @param u2 - input
 * @return 0 if valid; -1 if not
 */
int add_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2) {
    poly_n u;
    poly_n_add(&u, u1, u2);
    poly_n_reduce(&u);
    return poly_n_compare(&u, u0);
}


/**
 * Subtract commitments (u0 = u1 - u2), values and keys
 * @param ctx - context object
 * @param d - number of values
 * @param u0 - out commitment
 * @param v0 - out values
 * @param k0 - out key
 * @param u1 - in commitment 1
 * @param v1 - in values
 * @param k1 - in keys
 * @param u2 - in commitment 2
 * @param v2 - in values
 * @param k2 - in keys
 */
void sub_gen(context_t *ctx, uint8_t d,
             com_t *u0, zint_t *v0, mask_t *k0,
             com_t *u1, zint_t *v1, mask_t *k1,
             com_t *u2, zint_t *v2, mask_t *k2) {

    int j;
    poly_n_sub(u0, u1, u2);
    poly_n_reduce(u0);
    for (j = 0; j < d; j++)
        v0[j] = v1[j] - v2[j];

    for (j = 0; j < mk; j++)
        poly_sub(&k0->k[j], &k1->k[j], &k2->k[j]);
}

/**
 * Verifies whether u0 ?= u1 - u2
 * @param ctx - context
 * @param u0 - output to be verified
 * @param u1 - input
 * @param u2 - input
 * @return 0 if valid; -1 if not
 */
int sub_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2) {
    poly_n u;
    poly_n_sub(&u, u1, u2);
    poly_n_reduce(&u);
    return poly_n_compare(&u, u0);
}

/**
 * Allocate memory for a multiplication proof
 * @param proof - proof
 * @param d - number of integers
 */
int allocate_mul_proof(mul_pt *proof, uint8_t L1, uint8_t d) {
    if (d > mv)
        return -1;
    proof->z1 = (poly *) malloc(d * sizeof(poly));
    proof->z2 = (poly *) malloc(d * sizeof(poly));
    return 0;
}


/**
 * Deallocate memory of a multiplication proof
 * @param proof
 */
void deallocate_mul_proof(mul_pt *proof) {
    free(proof->z1);
    free(proof->z2);
}

/**
 * Multiply and generate proofs u0(v) = u1(v) * u2(v)
 * @param ctx - context object
 * @param d - number of values
 * @param proof - zero-knowledge proof
 * @param u0 - out commitment
 * @param v0 - out values
 * @param k0 - out key
 * @param u1 - in commitment 1
 * @param v1 - in values
 * @param k1 - in keys
 * @param u2 - in commitment 2
 * @param v2 - in values
 * @param k2 - in keys
 */
void mul_gen(context_t *ctx,  uint8_t L1, uint8_t d,
             mul_pt *proof,
             com_t *u0, zint_t *v0, mask_t *k0,
             com_t *u1, zint_t *v1, mask_t *k1,
             com_t *u2, zint_t *v2, mask_t *k2) {

    poly a1[d];
    poly a1_ntt[d];
    poly a2[d];
    poly a2_ntt[d];

    poly kt1[mk];
    poly kt2[mk];
    poly sv1[mv];
    poly sv2[mv];
    poly tmp1;
    poly tmp2;

    poly_n t2;
    poly_n t2_recreated;

    uint8_t l1_seed[(N/8 * (L1 + EXTRA_BITS + 1))];
    uint8_t tau_seed[r_BYTES];
    uint8_t tau1_seed[r1_BYTES];
    uint8_t tau2_seed[r2_BYTES];

    keccak_state state;
    poly x0;
    poly x0_ntt;
    poly x1;
    poly x1_ntt;
    poly x2;
    poly x2_ntt;
    poly x22_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t u0_bytes[u_chal_HIGHBITS];
    uint8_t t1_bytes[t1_HIGHBITS];
    uint8_t t2_bytes[aggr_HIGHBITS];
    uint8_t x0_bytes[SEED_BYTES];
    uint8_t x1_bytes[SEED_BYTES];

    poly_n tmp;
    poly_n tmpn1;
    poly_n tmpn2;
    int j;

    for (j = 0; j < d; j++) {
        v0[j] = v1[j] * v2[j];
    }

    proof->L1 = L1;
    proof->d = d;

    rejection_point:

    // Get keys
    for (j = 0; j < mk; j++) {
        get_mask_tau(&k0->k[j], tau_seed);
        get_mask_tau1(&kt1[j], tau1_seed);
        get_mask_tau2(&kt2[j], tau2_seed);
    }

    // Output commitment
    commit_from_int(ctx, d, u0, k0->k, v0);

    // value mask
    for (j = 0; j < d; j++) {
        get_alpha1_mask(&a1[j], v1[j], L1);
        get_alpha1_mask(&a2[j], v2[j], L1);
        poly_set(&a1_ntt[j], &a1[j]);
        poly_set(&a2_ntt[j], &a2[j]);
        poly_ntt(&a1_ntt[j]);
        poly_ntt(&a2_ntt[j]);
    }

    // sv1 = [sum v1a2 + v2a1]^d , 0^{mv - d}
    for (j = 0; j < d; j++) {
        poly_easy_mul(&tmp1, &a1[j], 0, v2[j]);
        poly_easy_mul(&tmp2, &a2[j], 0, v1[j]);
        poly_add(&sv1[j], &tmp1, &tmp2);
    }
    for (j = d; j < mv; j++) poly_set_zero(&sv1[j], 0, N);

    // t1
    commit(ctx, &proof->t1, kt1, sv1, nullptr);
    poly_n_highbits(&proof->t1, &proof->t1, t1_ERROR);

    // Packing
    poly_n_highbits(&tmp, u0, u_chal_ERROR);
    pack_poly_ring_custom(u0_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t1_bytes, &proof->t1, K1 - t1_ERROR);

    // Challenge 0
    shake256_init(&state);
    shake256_absorb(&state, u0_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t1_bytes, t1_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x0_bytes, SEED_BYTES, &state);
    poly_challenge(&x0, x0_bytes);
    poly_set(&x0_ntt, &x0);
    poly_ntt(&x0_ntt);

    // Challenge 1
    shake256_init(&state);
    shake256_absorb(&state, x0_bytes, SEED_BYTES);
    shake256_finalize(&state);
    shake256_squeeze(x1_bytes, SEED_BYTES, &state);
    poly_challenge(&x1, x1_bytes);
    poly_set(&x1_ntt, &x1);
    poly_ntt(&x1_ntt);

    // sv2 = [a1a2 + x0a1 + x1a2]^d, 0^{mv - d}
    for (j = 0; j < d; j++) {
        poly_pointwise_montgomery(&tmp1, &a1_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &a2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv2[j], &a1_ntt[j], &a2_ntt[j]);
        poly_reduce(&sv2[j]);
        poly_add(&sv2[j], &sv2[j], &tmp1); // a1a2 + x0a1 + x1a2 in NTT
        poly_inv_ntt_to_mont(&sv2[j]);
        poly_reduce(&sv2[j]);
    }
    for (j = d; j < mv; j++) poly_set_zero(&sv2[j], 0, N);

    // t2
    commit(ctx, &t2, kt2, sv2, nullptr);
    poly_n_highbits(&t2, &t2, aggr_ERROR);

    // Packing
    pack_poly_ring_custom(t2_bytes, &t2, K1 - aggr_ERROR);

    // Challenge 2
    shake256_init(&state);
    shake256_absorb(&state, x1_bytes, SEED_BYTES);
    shake256_absorb(&state, t2_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(proof->x2, SEED_BYTES, &state);
    poly_challenge(&x2, proof->x2);
    poly_set(&x2_ntt, &x2);
    poly_ntt(&x2_ntt);

    // z1 and z2
    zint_t SR_MUL_z = (((zint_t)1 << (L1 + EXTRA_BITS)) - ((zint_t)1 << (L1)));
    for (j = 0; j < d; j++) {
        // z1
        poly_easy_mul(&proof->z1[j], &x2, 0, v1[j]);
        poly_add(&proof->z1[j], &proof->z1[j], &a1[j]);
        // z2
        poly_easy_mul(&proof->z2[j], &x2, 0, v2[j]);
        poly_add(&proof->z2[j], &proof->z2[j], &a2[j]);

        if (poly_chknorm(&proof->z1[j], SR_MUL_z) != 0) {
            DEBUG_PRINT(("Rejected (mul) z1_%d\n", j));
            goto rejection_point;
        }

        if (poly_chknorm(&proof->z2[j], SR_MUL_z) != 0) {
            DEBUG_PRINT(("Rejected (mul) z2_%d\n", j));
            goto rejection_point;
        }
    }

    // s = x2^2k0 + x2(kt1 + x0k1 + x1k2)) + kt2
    poly x22;
    poly_pointwise_montgomery(&x22, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22);
    poly_ntt(&x22);
    for (j = 0; j < mk; j++) {
        poly_set(&proof->s[j], &k0->k[j]);
        poly_ntt(&proof->s[j]);
        poly_pointwise_montgomery(&proof->s[j], &proof->s[j], &x22); // x2^2k0
        poly_inv_ntt_to_mont(&proof->s[j]);

        poly_set(&tmp1, &k1->k[j]);
        poly_ntt(&tmp1);
        poly_pointwise_montgomery(&tmp1, &tmp1, &x0_ntt); // x0k1 in NTT

        poly_set(&tmp2, &k2->k[j]);
        poly_ntt(&tmp2);
        poly_pointwise_montgomery(&tmp2, &tmp2, &x1_ntt); // x1k2 in NTT

        poly_add(&tmp2, &tmp1, &tmp2);
        poly_inv_ntt_to_mont(&tmp2);
        poly_add(&tmp2, &tmp2, &kt1[j]); // kt1 + x0k1 + x1k2
        poly_ntt(&tmp2);
        poly_pointwise_montgomery(&tmp2, &tmp2, &x2_ntt); // x2(kt1 + x0k1 + x1k2) in NTT
        poly_inv_ntt_to_mont(&tmp2);
        poly_add(&proof->s[j], &proof->s[j], &tmp2);  // x2^2k0 + x2(kt1 + x0k1 + x1k2) in NTT
        poly_add(&proof->s[j], &proof->s[j], &kt2[j]); // x2^2k0 + x2(kt1 + x0k1 + x1k2) + kt2
        poly_reduce(&proof->s[j]);

        if (poly_chknorm(&proof->s[j], SR_MUL_s) != 0) {
            DEBUG_PRINT(("Rejected mul s_%d\n", j));
            goto rejection_point;
        }
    }

    // Verify the norm
    // s1 = sv = [z1z2 + x0z1 + x1z2]^d, 0^{mv - d}
    poly sv[d];
    poly z1_ntt[proof->d];
    poly z2_ntt[proof->d];
    for (j = 0; j < proof->d; j++) {
        poly_set(&z1_ntt[j], &proof->z1[j]);
        poly_set(&z2_ntt[j], &proof->z2[j]);
        poly_ntt(&z1_ntt[j]);
        poly_ntt(&z2_ntt[j]);

        poly_pointwise_montgomery(&tmp1, &z1_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &z2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv[j], &z1_ntt[j], &z2_ntt[j]);
        poly_reduce(&sv[j]);
        poly_add(&sv[j], &sv[j], &tmp1); // z1z2 + x0z1 + x1z2 in NTT
        poly_inv_ntt_to_mont(&sv[j]);
        poly_reduce(&sv[j]);

        if (poly_chknorm(&sv[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Rejected (mul) s1: sv_%d\n", j));
            goto rejection_point;
        }
    }

    // Creating hints
    for (j = proof->d; j < mv; j++) poly_set_zero(&sv[j], 0, N);

    commit(ctx, &t2_recreated, proof->s, sv, nullptr);

    poly_pointwise_montgomery(&x22_ntt, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22_ntt);
    poly_ntt(&x22_ntt);
    poly_n_set(&tmp, u0);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x22_ntt); // x2^2u0 in NTT
    poly_n_inv_ntt_to_mont(&tmp);

    poly_n_set(&tmpn1, u1);
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x0_ntt); // x0u1 in NTT

    poly_n_set(&tmpn2, u2);
    poly_n_ntt(&tmpn2);
    poly_n_pointwise_montgomery(&tmpn2, &tmpn2, &x1_ntt); // x1u2 in NTT

    poly_n_add(&tmpn1, &tmpn1, &tmpn2); // x0u1 + x1u2 in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_reduce(&tmpn1);

    poly_n_roundup(&tmpn2, &proof->t1, t1_ERROR);
    poly_n_add(&tmpn1, &tmpn1, &tmpn2);  // t1 + x0u1 + x1u2
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x2_ntt); // x2(t1 + x0u1 + x1u2) in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_add(&tmp, &tmp, &tmpn1); // x2^2u0 + x2(t1 + x0u1 + x1u2) in NTT
    poly_n_reduce(&tmp);

    poly_n_sub(&t2_recreated, &t2_recreated, &tmp);
    poly_n_reduce(&t2_recreated);
    poly_n_highbits(&t2_recreated, &t2_recreated, aggr_ERROR);

    int res = poly_n_hints(&proof->t2_hints, &t2_recreated, &t2);
    if (res == -1) {
        DEBUG_PRINT(("Rejected t2_hints (mul)\n"));
        goto rejection_point;
    }

    poly_n_makeup(&t2_recreated, &proof->t2_hints);
    if (poly_n_compare(&t2_recreated, &t2) != 0) {
        DEBUG_PRINT(("Not equal t2 (mul)\n"));
    }

}

/**
 * Verify the multiplication with the given proof
 * u0(v) ?= u1(v) * u2(v)
 * @param ctx - context object
 * @param u0  - output to be verified
 * @param u1 - input
 * @param u2 - input
 * @param proof - zero-knowledge proof
 * @return 0 - valid (-1) - invalid
 */
int mul_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2, mul_pt *proof) {
    poly tmp1;
    poly tmp2;

    poly_n t2;

    keccak_state state;
    poly x0;
    poly x0_ntt;
    poly x1;
    poly x1_ntt;
    poly x2;
    poly x2_ntt;
    poly x22_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t u0_bytes[u_chal_HIGHBITS];
    uint8_t t1_bytes[t1_HIGHBITS];
    uint8_t t2_bytes[aggr_HIGHBITS];
    uint8_t x0_bytes[SEED_BYTES];
    uint8_t x1_bytes[SEED_BYTES];
    uint8_t x2_bytes[SEED_BYTES];

    poly sv[mv];
    poly z1_ntt[proof->d];
    poly z2_ntt[proof->d];

    poly_n tmp;
    poly_n tmpn1;
    poly_n tmpn2;
    int j;

    zint_t ALPHA1 = (zint_t) 1 << (proof->L1 + EXTRA_BITS);
    for (j = 0; j < proof->d; j++) {
        if (poly_chknorm(&proof->z1[j], ALPHA1) != 0)
            return -1;
        if (poly_chknorm(&proof->z2[j], ALPHA1) != 0)
            return -1;
    }
    for (j = 0; j < mk; j++) {
        if (poly_chknorm(&proof->s[j], TAU2) != 0)
            return -1;
    }

    // Packing
    poly_n_highbits(&tmp, u0, u_chal_ERROR);
    pack_poly_ring_custom(u0_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t1_bytes, &proof->t1, K1 - t1_ERROR);

    // Challenge 0
    shake256_init(&state);
    shake256_absorb(&state, u0_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t1_bytes, t1_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x0_bytes, SEED_BYTES, &state);
    poly_challenge(&x0, x0_bytes);
    poly_set(&x0_ntt, &x0);
    poly_ntt(&x0_ntt);

    // Challenge 1
    shake256_init(&state);
    shake256_absorb(&state, x0_bytes, SEED_BYTES);
    shake256_finalize(&state);
    shake256_squeeze(x1_bytes, SEED_BYTES, &state);
    poly_challenge(&x1, x1_bytes);
    poly_set(&x1_ntt, &x1);
    poly_ntt(&x1_ntt);

    // Challenge 2
    poly_challenge(&x2, proof->x2);
    poly_set(&x2_ntt, &x2);
    poly_ntt(&x2_ntt);

    // sv = [z1z2 + x0z1 + x1z2]^d, 0^{mv - d}
    for (j = 0; j < proof->d; j++) {
        poly_set(&z1_ntt[j], &proof->z1[j]);
        poly_set(&z2_ntt[j], &proof->z2[j]);
        poly_ntt(&z1_ntt[j]);
        poly_ntt(&z2_ntt[j]);

        poly_pointwise_montgomery(&tmp1, &z1_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &z2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv[j], &z1_ntt[j], &z2_ntt[j]);
        poly_reduce(&sv[j]);
        poly_add(&sv[j], &sv[j], &tmp1); // z1z2 + x0z1 + x1z2 in NTT
        poly_inv_ntt_to_mont(&sv[j]);
        poly_reduce(&sv[j]);

        if (poly_chknorm(&sv[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Large norm (mul) s1: sv_%d\n", j));
            return -1;
        }
    }

    for (j = proof->d; j < mv; j++) poly_set_zero(&sv[j], 0, N);

    commit(ctx, &t2, proof->s, sv, nullptr);

    poly_pointwise_montgomery(&x22_ntt, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22_ntt);
    poly_ntt(&x22_ntt);
    poly_n_set(&tmp, u0);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x22_ntt); // x2^2u0 in NTT
    poly_n_inv_ntt_to_mont(&tmp);

    poly_n_set(&tmpn1, u1);
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x0_ntt); // x0u1 in NTT

    poly_n_set(&tmpn2, u2);
    poly_n_ntt(&tmpn2);
    poly_n_pointwise_montgomery(&tmpn2, &tmpn2, &x1_ntt); // x1u2 in NTT

    poly_n_add(&tmpn1, &tmpn1, &tmpn2); // x0u1 + x1u2 in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_reduce(&tmpn1);

    poly_n_roundup(&tmpn2, &proof->t1, t1_ERROR);
    poly_n_add(&tmpn1, &tmpn1, &tmpn2);  // t1 + x0u1 + x1u2
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x2_ntt); // x2(t1 + x0u1 + x1u2) in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_add(&tmp, &tmp, &tmpn1); // x2^2u0 + x2(t1 + x0u1 + x1u2) in NTT
    poly_n_reduce(&tmp);

    poly_n_sub(&t2, &t2, &tmp);
    poly_n_reduce(&t2);

    poly_n_highbits(&t2, &t2, aggr_ERROR);
    poly_n_makeup(&t2, &proof->t2_hints);
    pack_poly_ring_custom(t2_bytes, &t2, K1 - aggr_ERROR);

    shake256_init(&state);
    shake256_init(&state);
    shake256_absorb(&state, x1_bytes, SEED_BYTES);
    shake256_absorb(&state, t2_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x2_bytes, SEED_BYTES, &state);

    if (memcmp(x2_bytes, proof->x2, SEED_BYTES) == 0)
        return 0;

    return -1;
}


/**
 * Allocate memory for a "division" proof for unsigned numbers
 * @param proof - proof
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - number of integers
 */
int allocate_div_unsigned_proof(div_upt *proof, uint8_t L1, uint8_t d) {
    if (d > mv)
        return -1;
    proof->z0 = (poly *) malloc(d * sizeof(poly));
    proof->z2 = (poly *) malloc(d * sizeof(poly));
    allocate_rp(&proof->rp, L1, d);
    return 0;
}


/**
 * Deallocate memory of a "division" proof for unsigned numbers
 * @param proof
 */
void deallocate_div_unsigned_proof(div_upt *proof) {
    free(proof->z0);
    free(proof->z2);
    deallocate_rp(&proof->rp);
}

/**
 * Division and generate proofs u1(v) = u0(v) * u2(v) + u3(v)
 * Note that all numbers should be positive
 * @param ctx - context object
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - number of values
 * @param proof - zero-knowledge proof
 * @param u0 - out commitment
 * @param v0 - out values
 * @param k0 - out key
 * @param u3 - remainder commitment
 * @param v3 - remainder values
 * @param k3 - remainder key
 * @param u1 - in commitment 1
 * @param v1 - in values
 * @param k1 - in keys
 * @param u2 - in commitment 2
 * @param v2 - in values
 * @param k2 - in keys
 */
int div_unsigned_gen(context_t *ctx, uint8_t L1, uint8_t d,
                     div_upt *proof,
                     com_t *u0, zint_t *v0, mask_t *k0,
                     com_t *u3, zint_t *v3, mask_t *k3,
                     com_t *u1, zint_t *v1, mask_t *k1,
                     com_t *u2, zint_t *v2, mask_t *k2) {

    poly a0[d];
    poly a0_ntt[d];
    poly a2[d];
    poly a2_ntt[d];

    poly kt1[mk];
    poly kt2[mk];
    poly sv1[mv];
    poly sv2[mv];
    poly tmp1;
    poly tmp2;

    poly_n t2;
    poly_n t2_recreated;
    poly_n e;

    uint8_t tau_seed[r_BYTES];
    uint8_t tau1_seed[r1_BYTES];
    uint8_t tau2_seed[r2_BYTES];

    keccak_state state;
    poly x0;
    poly x0_ntt;
    poly x1;
    poly x1_ntt;
    poly x2;
    poly x2_ntt;
    poly x22_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t u0_bytes[u_chal_HIGHBITS];
    uint8_t u3_bytes[u_chal_HIGHBITS];
    uint8_t t1_bytes[t1_HIGHBITS];
    uint8_t t2_bytes[aggr_HIGHBITS];
    uint8_t x0_bytes[SEED_BYTES];
    uint8_t x1_bytes[SEED_BYTES];
    zint_t one[d];

    poly_n tmp;
    poly_n tmpn1;
    poly_n tmpn2;
    int j, val1, val2;

    for (j = 0; j < d; j++) {
        if (v1[j] < 0 || v2[j] <= 0)
            return -1;
        v0[j] = v1[j] / v2[j];
        v3[j] = v1[j] % v2[j];
    }

    proof->L1 = L1;
    proof->d = d;

    // Get keys
    for (j = 0; j < mk; j++) {
        get_mask_tau(&k0->k[j], tau_seed);
        get_mask_tau(&k3->k[j], tau_seed);
    }
    // Output and remainder commitment
    commit_from_int(ctx, d, u0, k0->k, v0);
    commit_from_int(ctx, d, u3, k3->k, v3);

    // leq proof for the remainder commitment
    for (j = 0; j < d; j++) one[j] = 1;
    commit_from_int(ctx, d, &e, nullptr, one);
    poly_n_add(&e, u3, &e);
    poly_n_reduce(&e);
    for (j = 0; j < d; j++) one[j] = v3[j] + 1;
    leq_gen(ctx, L1, d, &proof->rp, &e, one, k3, u2, v2, k2);

    rejection_point:

    // Get keys
    for (j = 0; j < mk; j++) {
        get_mask_tau1(&kt1[j], tau1_seed);
        get_mask_tau2(&kt2[j], tau2_seed);
    }

    // value mask
    for (j = 0; j < d; j++) {
        get_alpha1_mask(&a0[j], v0[j], L1);
        get_alpha1_mask(&a2[j], v2[j], L1);
        poly_set(&a0_ntt[j], &a0[j]);
        poly_set(&a2_ntt[j], &a2[j]);
        poly_ntt(&a0_ntt[j]);
        poly_ntt(&a2_ntt[j]);
    }

    // sv1 = [sum v1a2 + v2a1]^d , 0^{mv - d}
    for (j = 0; j < d; j++) {
        poly_easy_mul(&tmp1, &a0[j], 0, v2[j]);
        poly_easy_mul(&tmp2, &a2[j], 0, v0[j]);
        poly_add(&sv1[j], &tmp1, &tmp2);
    }

    for (j = d; j < mv; j++) poly_set_zero(&sv1[j], 0, N);

    // t1
    commit(ctx, &proof->t1, kt1, sv1, nullptr);
    poly_n_highbits(&proof->t1, &proof->t1, t1_ERROR);

    poly_n_highbits(&tmp, u0, u_chal_ERROR);
    pack_poly_ring_custom(u0_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u3, u_chal_ERROR);
    pack_poly_ring_custom(u3_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t1_bytes, &proof->t1, K1 - t1_ERROR);

    // Challenge 0
    shake256_init(&state);
    shake256_absorb(&state, u0_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u3_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t1_bytes, t1_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x0_bytes, SEED_BYTES, &state);
    poly_challenge(&x0, x0_bytes);
    poly_set(&x0_ntt, &x0);
    poly_ntt(&x0_ntt);

    // Challenge 1
    shake256_init(&state);
    shake256_absorb(&state, x0_bytes, SEED_BYTES);
    shake256_finalize(&state);
    shake256_squeeze(x1_bytes, SEED_BYTES, &state);
    poly_challenge(&x1, x1_bytes);
    poly_set(&x1_ntt, &x1);
    poly_ntt(&x1_ntt);

    // sv2 = [a1a2 + x0a0 + x1a2)]^d, 0^{mv - d}
    for (j = 0; j < d; j++) {
        poly_pointwise_montgomery(&tmp1, &a0_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &a2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv2[j], &a0_ntt[j], &a2_ntt[j]);
        poly_reduce(&sv2[j]);
        poly_add(&sv2[j], &sv2[j], &tmp1); // a1a2 + x0a0 + x1a2 in NTT
        poly_inv_ntt_to_mont(&sv2[j]);
        poly_reduce(&sv2[j]);

    }
    for (j = d; j < mv; j++) poly_set_zero(&sv2[j], 0, N);

    // t2
    commit(ctx, &t2, kt2, sv2, nullptr);
    poly_n_highbits(&t2, &t2, aggr_ERROR);
    pack_poly_ring_custom(t2_bytes, &t2, K1 - aggr_ERROR);

    // Challenge 2
    shake256_init(&state);
    shake256_absorb(&state, x1_bytes, SEED_BYTES);
    shake256_absorb(&state, t2_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(proof->x2, SEED_BYTES, &state);
    poly_challenge(&x2, proof->x2);
    poly_set(&x2_ntt, &x2);
    poly_ntt(&x2_ntt);

    // z1 and z2
    zint_t SR_DIV_z = (((zint_t)1 << (L1 + EXTRA_BITS)) - ((zint_t)1 << (L1)));
    for (j = 0; j < d; j++) {
        // z1
        poly_easy_mul(&proof->z0[j], &x2, 0, v0[j]);
        poly_add(&proof->z0[j], &proof->z0[j], &a0[j]);
        // z2
        poly_easy_mul(&proof->z2[j], &x2, 0, v2[j]);
        poly_add(&proof->z2[j], &proof->z2[j], &a2[j]);

        if (poly_chknorm(&proof->z0[j], SR_DIV_z) != 0) {
            DEBUG_PRINT(("Rejected (unsigned div) z0_%d\n", j));
            goto rejection_point;
        }

        if (poly_chknorm(&proof->z2[j], SR_DIV_z) != 0) {
            DEBUG_PRINT(("Rejected  (unsigned div) z2_%d\n", j));
            goto rejection_point;
        }
    }

    // s = x2^2(k1 - k3) + x2(kt1 + x1(k0 + k2)) + kt2
    poly x22;
    poly_pointwise_montgomery(&x22, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22);
    poly_ntt(&x22);
    for (j = 0; j < mk; j++) {
        poly_sub(&proof->s[j], &k1->k[j], &k3->k[j]); // k1 - k3
        poly_ntt(&proof->s[j]);
        poly_pointwise_montgomery(&proof->s[j], &proof->s[j], &x22); // x2^2(k1 - k3)
        poly_inv_ntt_to_mont(&proof->s[j]);

        poly_set(&tmp1, &k0->k[j]);
        poly_ntt(&tmp1);
        poly_pointwise_montgomery(&tmp1, &tmp1, &x0_ntt); // x0k0 in NTT

        poly_set(&tmp2, &k2->k[j]);
        poly_ntt(&tmp2);
        poly_pointwise_montgomery(&tmp2, &tmp2, &x1_ntt); // x1k2 in NTT

        poly_add(&tmp2, &tmp1, &tmp2);
        poly_inv_ntt_to_mont(&tmp2);
        poly_add(&tmp2, &tmp2, &kt1[j]); // kt1 + x0k0 + x1k2
        poly_ntt(&tmp2);
        poly_pointwise_montgomery(&tmp2, &tmp2, &x2_ntt); // x2(kt1 + x0k0 + x1k2) in NTT
        poly_inv_ntt_to_mont(&tmp2);
        poly_add(&proof->s[j], &proof->s[j], &tmp2);  // x2^2k0 + x2(kt1 + x0k0 + x1k2) in NTT
        poly_add(&proof->s[j], &proof->s[j], &kt2[j]); // x2^2k0 + x2(kt1 + x0k0 + x1k2) + kt2
        poly_reduce(&proof->s[j]);

        if (poly_chknorm(&proof->s[j], SR_MUL_s) != 0) {
            DEBUG_PRINT(("Rejected (unsigned div) s_%d\n", j));
            goto rejection_point;
        }
    }

    // sv = [z0z2 + x0z0 + x1z2]^d, 0^{mv - d}
    poly sv[d];
    poly z0_ntt[proof->rp.d];
    poly z2_ntt[proof->rp.d];
    for (j = 0; j < proof->rp.d; j++) {
        poly_set(&z0_ntt[j], &proof->z0[j]);
        poly_set(&z2_ntt[j], &proof->z2[j]);
        poly_ntt(&z0_ntt[j]);
        poly_ntt(&z2_ntt[j]);

        poly_pointwise_montgomery(&tmp1, &z0_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &z2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv[j], &z0_ntt[j], &z2_ntt[j]);
        poly_reduce(&sv[j]);
        poly_add(&sv[j], &sv[j], &tmp1); // z0z2 + x0z0 + x1z2 in NTT
        poly_inv_ntt_to_mont(&sv[j]);
        poly_reduce(&sv[j]);

        if (poly_chknorm(&sv[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Rejected (div unsigned) sv_%d\n", j));
            goto rejection_point;
        }
    }

    // Creating Hints
    for (j = proof->rp.d; j < mv; j++) poly_set_zero(&sv[j], 0, N);

    commit(ctx, &t2_recreated, proof->s, sv, nullptr);

    poly_pointwise_montgomery(&x22_ntt, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22_ntt);
    poly_ntt(&x22_ntt);
    poly_n_sub(&tmp, u1, u3); // u1 - u3
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x22_ntt); // x2^2(u1 - u3) in NTT
    poly_n_inv_ntt_to_mont(&tmp);

    poly_n_set(&tmpn1, u0);
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x0_ntt); // x0u0 in NTT

    poly_n_set(&tmpn2, u2);
    poly_n_ntt(&tmpn2);
    poly_n_pointwise_montgomery(&tmpn2, &tmpn2, &x1_ntt); // x1u2 in NTT

    poly_n_add(&tmpn1, &tmpn1, &tmpn2); // x0u0 + x1u2 in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_reduce(&tmpn1);

    poly_n_roundup(&tmpn2, &proof->t1, t1_ERROR);
    poly_n_add(&tmpn1, &tmpn1, &tmpn2);  // t1 + x0u0 + x1u2
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x2_ntt); // x2(t1 + x0u0 + x1u2) in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_add(&tmp, &tmp, &tmpn1); // x2^2(u1 - u3) + x2(t1 + x0u0 + x1u2) in NTT
    poly_n_reduce(&tmp);

    poly_n_sub(&t2_recreated, &t2_recreated, &tmp);
    poly_n_reduce(&t2_recreated);
    poly_n_highbits(&t2_recreated, &t2_recreated, aggr_ERROR);

    int res = poly_n_hints(&proof->t2_hints, &t2_recreated, &t2);
    if (res == -1) {
        DEBUG_PRINT(("Rejected t2_hints (unsigned div)\n"));
        goto rejection_point;
    }

    poly_n_makeup(&t2_recreated, &proof->t2_hints);
    if (poly_n_compare(&t2_recreated, &t2) != 0) {
        DEBUG_PRINT(("Not equal (unsigned div)\n"));
    }

    return 0;
}

/**
 * Verifies a division such that u1(v) ?= u0(v)*u2(v) + u3(v) when u3(v) < u2(v)
 * of unsigned numbers
 * @param ctx - context object
 * @param u0 - output
 * @param u1 - input
 * @param u2 - input
 * @param u3 - remainder
 * @param proof - zero-knowledge proof
 * @return 0 - valid, (-1) - invalid
 */
int div_unsigned_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2, com_t *u3, div_upt *proof) {
    poly tmp1;
    poly tmp2;

    poly_n t2;
    poly_n e;

    keccak_state state;
    poly x0;
    poly x0_ntt;
    poly x1;
    poly x1_ntt;
    poly x2;
    poly x2_ntt;
    poly x22_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t u0_bytes[u_chal_HIGHBITS];
    uint8_t u3_bytes[u_chal_HIGHBITS];
    uint8_t t1_bytes[t1_HIGHBITS];
    uint8_t t2_bytes[aggr_HIGHBITS];
    uint8_t x0_bytes[SEED_BYTES];
    uint8_t x1_bytes[SEED_BYTES];
    uint8_t x2_bytes[SEED_BYTES];

    poly sv[mv];
    poly z0_ntt[proof->rp.d];
    poly z2_ntt[proof->rp.d];

    zint_t one[proof->rp.d];

    poly_n tmp;
    poly_n tmpn1;
    poly_n tmpn2;
    int j;

    // leq proof for the remainder commitment
    for (j = 0; j < proof->rp.d; j++) one[j] = 1;
    commit_from_int(ctx, proof->rp.d, &e, nullptr, one);
    poly_n_add(&e, u3, &e);
    poly_n_reduce(&e);
    if(leq_ver(ctx, &e, u2, &proof->rp) != 0)
        return -1;

    zint_t ALPHA1 = (zint_t) 1 << (proof->L1 + EXTRA_BITS);
    for (j = 0; j < proof->rp.d; j++) {
        if (poly_chknorm(&proof->z0[j], ALPHA1) != 0)
            return -1;
        if (poly_chknorm(&proof->z2[j], ALPHA1) != 0)
            return -1;
    }
    for (j = 0; j < mk; j++) {
        if (poly_chknorm(&proof->s[j], TAU2) != 0)
            return -1;
    }

    poly_n_highbits(&tmp, u0, u_chal_ERROR);
    pack_poly_ring_custom(u0_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u3, u_chal_ERROR);
    pack_poly_ring_custom(u3_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t1_bytes, &proof->t1, K1 - t1_ERROR);

    // Challenge 0
    shake256_init(&state);
    shake256_absorb(&state, u0_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u3_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t1_bytes, t1_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x0_bytes, SEED_BYTES, &state);
    poly_challenge(&x0, x0_bytes);
    poly_set(&x0_ntt, &x0);
    poly_ntt(&x0_ntt);

    // Challenge 1
    shake256_init(&state);
    shake256_absorb(&state, x0_bytes, SEED_BYTES);
    shake256_finalize(&state);
    shake256_squeeze(x1_bytes, SEED_BYTES, &state);
    poly_challenge(&x1, x1_bytes);
    poly_set(&x1_ntt, &x1);
    poly_ntt(&x1_ntt);

    // Challenge 2
    poly_challenge(&x2, proof->x2);
    poly_set(&x2_ntt, &x2);
    poly_ntt(&x2_ntt);

    // sv = [z1z2 + x0z0 + x1z2]^d, 0^{mv - d}
    for (j = 0; j < proof->rp.d; j++) {
        poly_set(&z0_ntt[j], &proof->z0[j]);
        poly_set(&z2_ntt[j], &proof->z2[j]);
        poly_ntt(&z0_ntt[j]);
        poly_ntt(&z2_ntt[j]);

        poly_pointwise_montgomery(&tmp1, &z0_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &z2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv[j], &z0_ntt[j], &z2_ntt[j]);
        poly_reduce(&sv[j]);
        poly_add(&sv[j], &sv[j], &tmp1); // z0z2 + x0z0 + x1z2 in NTT
        poly_inv_ntt_to_mont(&sv[j]);
        poly_reduce(&sv[j]);

        if (poly_chknorm(&sv[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Large norm (div signed) sv_%d\n", j));
            return -1;
        }
    }

    for (j = proof->rp.d; j < mv; j++) poly_set_zero(&sv[j], 0, N);

    commit(ctx, &t2, proof->s, sv, nullptr);

    poly_pointwise_montgomery(&x22_ntt, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22_ntt);
    poly_ntt(&x22_ntt);
    poly_n_sub(&tmp, u1, u3); // u1 - u3
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x22_ntt); // x2^2(u1 - u3) in NTT
    poly_n_inv_ntt_to_mont(&tmp);

    poly_n_set(&tmpn1, u0);
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x0_ntt); // x0u0 in NTT

    poly_n_set(&tmpn2, u2);
    poly_n_ntt(&tmpn2);
    poly_n_pointwise_montgomery(&tmpn2, &tmpn2, &x1_ntt); // x1u2 in NTT

    poly_n_add(&tmpn1, &tmpn1, &tmpn2); // x0u0 + x1u2 in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_reduce(&tmpn1);

    poly_n_roundup(&tmpn2, &proof->t1, t1_ERROR);
    poly_n_add(&tmpn1, &tmpn1, &tmpn2);  // t1 + x0u0 + x1u2
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x2_ntt); // x2(t1 + x0u0 + x1u2) in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_add(&tmp, &tmp, &tmpn1); // x2^2(u1 - u3) + x2(t1 + x0u0 + x1u2) in NTT
    poly_n_reduce(&tmp);

    poly_n_sub(&t2, &t2, &tmp);
    poly_n_reduce(&t2);

    poly_n_highbits(&t2, &t2, aggr_ERROR);
    poly_n_makeup(&t2, &proof->t2_hints);

    pack_poly_ring_custom(t2_bytes, &t2, K1 - aggr_ERROR);
    shake256_init(&state);
    shake256_absorb(&state, x1_bytes, SEED_BYTES);
    shake256_absorb(&state, t2_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x2_bytes, SEED_BYTES, &state);

    if (memcmp(x2_bytes, proof->x2, SEED_BYTES) == 0)
        return 0;

    return -1;
}


/**
 * Allocate memory for a "division" proof for signed numbers
 * @param proof - proof
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - number of integers
 */
int allocate_div_signed_proof(div_pt *proof, uint8_t L1, uint8_t d) {
    if (d > mv)
        return -1;
    proof->z0 = (poly *) malloc(d * sizeof(poly));
    proof->z2 = (poly *) malloc(d * sizeof(poly));
    allocate_rp(&proof->rp, 2*L1, d);
    allocate_rp(&proof->rp_v3, L1, d);
    allocate_mul_proof(&proof->u2_proof, L1, d);
    allocate_mul_proof(&proof->u3_proof, L1, d);
    return 0;
}


/**
 * Deallocate memory of a "division" proof for signed numbers
 * @param proof
 */
void deallocate_div_signed_proof(div_pt *proof) {
    free(proof->z0);
    free(proof->z2);
    deallocate_rp(&proof->rp);
    deallocate_rp(&proof->rp_v3);
    deallocate_mul_proof(&proof->u2_proof);
    deallocate_mul_proof(&proof->u3_proof);
}

/**
 * Division and generate proofs u1(v) = u0(v) * u2(v) + u3(v)
 * @param ctx - context object
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - number of values
 * @param proof - zero-knowledge proof
 * @param u0 - out commitment
 * @param v0 - out values
 * @param k0 - out key
 * @param u3 - remainder commitment
 * @param v3 - remainder values
 * @param k3 - remainder key
 * @param u1 - in commitment 1
 * @param v1 - in values
 * @param k1 - in keys
 * @param u2 - in commitment 2
 * @param v2 - in values
 * @param k2 - in keys
 */
int div_signed_gen(context_t *ctx, uint8_t L1, uint8_t d,
                   div_pt *proof,
                   com_t *u0, zint_t *v0, mask_t *k0,
                   com_t *u3, zint_t *v3, mask_t *k3,
                   com_t *u1, zint_t *v1, mask_t *k1,
                   com_t *u2, zint_t *v2, mask_t *k2) {

    poly a0[d];
    poly a0_ntt[d];
    poly a2[d];
    poly a2_ntt[d];

    poly kt1[mk];
    poly kt2[mk];
    poly sv1[mv];
    poly sv2[mv];
    poly tmp1;
    poly tmp2;

    poly_n t2;
    poly_n t2_recreated;

    poly_n e;
    mask_t k22;
    mask_t k32;
    zint_t v22[d];
    zint_t v32[d];

    uint8_t tau_seed[r_BYTES];
    uint8_t tau1_seed[r1_BYTES];
    uint8_t tau2_seed[r2_BYTES];

    keccak_state state;
    poly x0;
    poly x0_ntt;
    poly x1;
    poly x1_ntt;
    poly x2;
    poly x2_ntt;
    poly x22_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t u0_bytes[u_chal_HIGHBITS];
    uint8_t u3_bytes[u_chal_HIGHBITS];
    uint8_t t1_bytes[t1_HIGHBITS];
    uint8_t t2_bytes[aggr_HIGHBITS];
    uint8_t x0_bytes[SEED_BYTES];
    uint8_t x1_bytes[SEED_BYTES];
    zint_t one[d];

    poly_n tmp;
    poly_n tmpn1;
    poly_n tmpn2;
    int j, val1, val2;

    for (j = 0; j < d; j++) {
        if (v2[j] == 0)
            return -1;
        v0[j] = v1[j] / v2[j];
        v3[j] = v1[j] - (v0[j]*v2[j]);
        // v3 should be positive
        if (v3[j] < 0)
            return -1;
    }

    proof->L1 = L1;
    proof->d = d;

    // Get keys
    for (j = 0; j < mk; j++) {
        get_mask_tau(&k0->k[j], tau_seed);
        get_mask_tau(&k3->k[j], tau_seed);
    }

    // Output and remainder commitment
    commit_from_int(ctx, d, u0, k0->k, v0);
    commit_from_int(ctx, d, u3, k3->k, v3);

    // leq proof for the remainder commitment
    // v2^2 - (v3 + 1)^1 >= 0
    for (j = 0; j < d; j++) one[j] = 1;
    commit_from_int(ctx, d, &e, nullptr, one);
    poly_n_add(&e, u3, &e);
    poly_n_reduce(&e);
    for (j = 0; j < d; j++) one[j] = v3[j] + 1;
    // v2^2
    mul_gen(ctx, L1, d, &proof->u2_proof, &proof->u22, v22, &k22,  u2, v2, k2, u2, v2, k2);
    // (v3 + 1)^2
    mul_gen(ctx, L1, d, &proof->u3_proof, &proof->u32, v32, &k32,  &e, one, k3, &e, one, k3);
    leq_gen(ctx, L1, d, &proof->rp, &proof->u32, v32, &k32, &proof->u22, v22, &k22);


    // range proof for v3
    range_gen(ctx, L1, d, &proof->rp_v3, u3, v3, k3->k);

    rejection_point:

    // Get keys
    for (j = 0; j < mk; j++) {
        get_mask_tau1(&kt1[j], tau1_seed);
        get_mask_tau2(&kt2[j], tau2_seed);
    }

    // value mask
    for (j = 0; j < d; j++) {
        get_alpha1_mask(&a0[j], v0[j], L1);
        get_alpha1_mask(&a2[j], v2[j], L1);
        poly_set(&a0_ntt[j], &a0[j]);
        poly_set(&a2_ntt[j], &a2[j]);
        poly_ntt(&a0_ntt[j]);
        poly_ntt(&a2_ntt[j]);
    }

    // sv1 = [sum v1a2 + v2a1]^d , 0^{mv - d}
    for (j = 0; j < d; j++) {
        poly_easy_mul(&tmp1, &a0[j], 0, v2[j]);
        poly_easy_mul(&tmp2, &a2[j], 0, v0[j]);
        poly_add(&sv1[j], &tmp1, &tmp2);
    }
    for (j = d; j < mv; j++) poly_set_zero(&sv1[j], 0, N);

    // t1
    commit(ctx, &proof->t1, kt1, sv1, nullptr);
    poly_n_highbits(&proof->t1, &proof->t1, t1_ERROR);

    poly_n_highbits(&tmp, u0, u_chal_ERROR);
    pack_poly_ring_custom(u0_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u3, u_chal_ERROR);
    pack_poly_ring_custom(u3_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t1_bytes, &proof->t1, K1 - t1_ERROR);

    // Challenge 0
    shake256_init(&state);
    shake256_absorb(&state, u0_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u3_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t1_bytes, t1_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x0_bytes, SEED_BYTES, &state);
    poly_challenge(&x0, x0_bytes);
    poly_set(&x0_ntt, &x0);
    poly_ntt(&x0_ntt);

    // Challenge 1
    shake256_init(&state);
    shake256_absorb(&state, x0_bytes, SEED_BYTES);
    shake256_finalize(&state);
    shake256_squeeze(x1_bytes, SEED_BYTES, &state);
    poly_challenge(&x1, x1_bytes);
    poly_set(&x1_ntt, &x1);
    poly_ntt(&x1_ntt);

    // sv2 = [a1a2 + x0a0 + x1a2]^d, 0^{mv - d}
    for (j = 0; j < d; j++) {
        poly_pointwise_montgomery(&tmp1, &a0_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &a2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv2[j], &a0_ntt[j], &a2_ntt[j]);
        poly_reduce(&sv2[j]);
        poly_add(&sv2[j], &sv2[j], &tmp1); // a1a2 + x0a0 + x1a2 in NTT
        poly_inv_ntt_to_mont(&sv2[j]);
        poly_reduce(&sv2[j]);

        /*if (poly_chknorm(&sv2[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Rejected sv2\n");
            goto rejection_point;
        }*/
    }
    for (j = d; j < mv; j++) poly_set_zero(&sv2[j], 0, N);

    // t2
    commit(ctx, &t2, kt2, sv2, nullptr);
    poly_n_highbits(&t2, &t2, aggr_ERROR);
    pack_poly_ring_custom(t2_bytes, &t2, K1 - aggr_ERROR);

    // Challenge 2
    shake256_init(&state);
    shake256_absorb(&state, x1_bytes, SEED_BYTES);
    shake256_absorb(&state, t2_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(proof->x2, SEED_BYTES, &state);
    poly_challenge(&x2, proof->x2);
    poly_set(&x2_ntt, &x2);
    poly_ntt(&x2_ntt);

    // z1 and z2
    zint_t SR_DIV_z = (((zint_t)1 << (L1 + EXTRA_BITS)) - ((zint_t)1 << (L1)));
    for (j = 0; j < d; j++) {
        // z1
        poly_easy_mul(&proof->z0[j], &x2, 0, v0[j]);
        poly_add(&proof->z0[j], &proof->z0[j], &a0[j]);
        // z2
        poly_easy_mul(&proof->z2[j], &x2, 0, v2[j]);
        poly_add(&proof->z2[j], &proof->z2[j], &a2[j]);

        if (poly_chknorm(&proof->z0[j], SR_DIV_z) != 0) {
            DEBUG_PRINT(("Rejected  (signed div) z0_%d\n", j));
            goto rejection_point;
        }

        if (poly_chknorm(&proof->z2[j], SR_DIV_z) != 0) {
            DEBUG_PRINT(("Rejected  (signed div) z2_%d\n", j));
            goto rejection_point;
        }
    }

    // s = x2^2(k1 - k3) + x2(kt1 + x1(k0 + k2)) + kt2
    poly x22;
    poly_pointwise_montgomery(&x22, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22);
    poly_ntt(&x22);
    for (j = 0; j < mk; j++) {
        poly_sub(&proof->s[j], &k1->k[j], &k3->k[j]); // k1 - k3
        poly_ntt(&proof->s[j]);
        poly_pointwise_montgomery(&proof->s[j], &proof->s[j], &x22); // x2^2(k1 - k3)
        poly_inv_ntt_to_mont(&proof->s[j]);

        poly_set(&tmp1, &k0->k[j]);
        poly_ntt(&tmp1);
        poly_pointwise_montgomery(&tmp1, &tmp1, &x0_ntt); // x0k0 in NTT

        poly_set(&tmp2, &k2->k[j]);
        poly_ntt(&tmp2);
        poly_pointwise_montgomery(&tmp2, &tmp2, &x1_ntt); // x1k2 in NTT

        poly_add(&tmp2, &tmp1, &tmp2);
        poly_inv_ntt_to_mont(&tmp2);
        poly_add(&tmp2, &tmp2, &kt1[j]); // kt1 + x0k0 + x1k2
        poly_ntt(&tmp2);
        poly_pointwise_montgomery(&tmp2, &tmp2, &x2_ntt); // x2(kt1 + x0k0 + x1k2) in NTT
        poly_inv_ntt_to_mont(&tmp2);
        poly_add(&proof->s[j], &proof->s[j], &tmp2);  // x2^2k0 + x2(kt1 + x0k0 + x1k2) in NTT
        poly_add(&proof->s[j], &proof->s[j], &kt2[j]); // x2^2k0 + x2(kt1 + x0k0 + x1k2) + kt2
        poly_reduce(&proof->s[j]);

        if (poly_chknorm(&proof->s[j], SR_MUL_s) != 0) {
            DEBUG_PRINT(("Rejected  (signed div) s_%d\n", j));
            goto rejection_point;
        }
    }

    // sv = [z0z2 + x0z0 + x1z2]^d, 0^{mv - d}
    poly sv[d];
    poly z0_ntt[proof->rp.d];
    poly z2_ntt[proof->rp.d];
    for (j = 0; j < proof->rp.d; j++) {
        poly_set(&z0_ntt[j], &proof->z0[j]);
        poly_set(&z2_ntt[j], &proof->z2[j]);
        poly_ntt(&z0_ntt[j]);
        poly_ntt(&z2_ntt[j]);

        poly_pointwise_montgomery(&tmp1, &z0_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &z2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv[j], &z0_ntt[j], &z2_ntt[j]);
        poly_reduce(&sv[j]);
        poly_add(&sv[j], &sv[j], &tmp1); // z1z2 + x0z0 + x1z2 in NTT
        poly_inv_ntt_to_mont(&sv[j]);
        poly_reduce(&sv[j]);

        if (poly_chknorm(&sv[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Rejected (div signed) sv_%d\n", j));
            goto rejection_point;
        }
    }

    // Creating Hints
    for (j = proof->rp.d; j < mv; j++) poly_set_zero(&sv[j], 0, N);

    commit(ctx, &t2_recreated, proof->s, sv, nullptr);

    poly_pointwise_montgomery(&x22_ntt, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22_ntt);
    poly_ntt(&x22_ntt);
    poly_n_sub(&tmp, u1, u3); // u1 - u3
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x22_ntt); // x2^2(u1 - u3) in NTT
    poly_n_inv_ntt_to_mont(&tmp);

    poly_n_set(&tmpn1, u0);
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x0_ntt); // x0u0 in NTT

    poly_n_set(&tmpn2, u2);
    poly_n_ntt(&tmpn2);
    poly_n_pointwise_montgomery(&tmpn2, &tmpn2, &x1_ntt); // x1u2 in NTT

    poly_n_add(&tmpn1, &tmpn1, &tmpn2); // x0u0 + x1u2 in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_reduce(&tmpn1);

    poly_n_roundup(&tmpn2, &proof->t1, t1_ERROR);
    poly_n_add(&tmpn1, &tmpn1, &tmpn2);  // t1 + x0u0 + x1u2
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x2_ntt); // x2(t1 + x0u0 + x1u2) in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_add(&tmp, &tmp, &tmpn1); // x2^2(u1 - u3) + x2(t1 + x0u0 + x1u2) in NTT
    poly_n_reduce(&tmp);

    poly_n_sub(&t2_recreated, &t2_recreated, &tmp);
    poly_n_reduce(&t2_recreated);
    poly_n_highbits(&t2_recreated, &t2_recreated, aggr_ERROR);

    int res = poly_n_hints(&proof->t2_hints, &t2_recreated, &t2);
    if (res == -1) {
        DEBUG_PRINT(("Rejected t2_hints (signed div)\n"));
        goto rejection_point;
    }

    poly_n_makeup(&t2_recreated, &proof->t2_hints);
    if (poly_n_compare(&t2_recreated, &t2) != 0) {
        DEBUG_PRINT(("Not equal (signed div)\n"));
    }

    return 0;
}


/**
 * Verifies a division such that u1(v) ?= u0(v)*u2(v) + u3(v) when u3(v) < u2(v)
 * of signed numbers
 * @param ctx - context object
 * @param u0 - output
 * @param u1 - input
 * @param u2 - input
 * @param u3 - remainder
 * @param proof - zero-knowledge proof
 * @return 0 - valid, (-1) - invalid
 */
int div_signed_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2, com_t *u3, div_pt *proof) {
    poly tmp1;
    poly tmp2;

    poly_n t2;
    poly_n e;

    keccak_state state;
    poly x0;
    poly x0_ntt;
    poly x1;
    poly x1_ntt;
    poly x2;
    poly x2_ntt;
    poly x22_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t u0_bytes[u_chal_HIGHBITS];
    uint8_t u3_bytes[u_chal_HIGHBITS];
    uint8_t t1_bytes[t1_HIGHBITS];
    uint8_t t2_bytes[aggr_HIGHBITS];
    uint8_t x0_bytes[SEED_BYTES];
    uint8_t x1_bytes[SEED_BYTES];
    uint8_t x2_bytes[SEED_BYTES];

    poly sv[mv];
    poly z0_ntt[proof->rp.d];
    poly z2_ntt[proof->rp.d];

    zint_t one[proof->rp.d];

    poly_n tmp;
    poly_n tmpn1;
    poly_n tmpn2;
    int j;

    // leq proof for the remainder commitment
    // v2^2 - (v3 + 1)^1 >= 0
    for (j = 0; j < proof->rp.d; j++) one[j] = 1;
    commit_from_int(ctx, proof->rp.d, &e, nullptr, one);
    poly_n_add(&e, u3, &e);
    poly_n_reduce(&e);
    if (mul_ver(ctx, &proof->u22, u2, u2, &proof->u2_proof) != 0
        || mul_ver(ctx, &proof->u32, &e, &e, &proof->u3_proof) != 0
        || leq_ver(ctx, &proof->u32, &proof->u22, &proof->rp) != 0
        || range_ver(ctx, u3, &proof->rp_v3) != 0) {
        return -1;
    }

    zint_t ALPHA1 = (zint_t) 1 << (proof->L1 + EXTRA_BITS);
    for (j = 0; j < proof->rp.d; j++) {
        if (poly_chknorm(&proof->z0[j], ALPHA1) != 0)
            return -1;
        if (poly_chknorm(&proof->z2[j], ALPHA1) != 0)
            return -1;
    }
    for (j = 0; j < mk; j++) {
        if (poly_chknorm(&proof->s[j], TAU2) != 0)
            return -1;
    }


    poly_n_highbits(&tmp, u0, u_chal_ERROR);
    pack_poly_ring_custom(u0_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u3, u_chal_ERROR);
    pack_poly_ring_custom(u3_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t1_bytes, &proof->t1, K1 - t1_ERROR);

    // Challenge 0
    shake256_init(&state);
    shake256_absorb(&state, u0_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u3_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t1_bytes, t1_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x0_bytes, SEED_BYTES, &state);
    poly_challenge(&x0, x0_bytes);
    poly_set(&x0_ntt, &x0);
    poly_ntt(&x0_ntt);

    // Challenge 1
    shake256_init(&state);
    shake256_absorb(&state, x0_bytes, SEED_BYTES);
    shake256_finalize(&state);
    shake256_squeeze(x1_bytes, SEED_BYTES, &state);
    poly_challenge(&x1, x1_bytes);
    poly_set(&x1_ntt, &x1);
    poly_ntt(&x1_ntt);

    // Challenge 2
    poly_challenge(&x2, proof->x2);
    poly_set(&x2_ntt, &x2);
    poly_ntt(&x2_ntt);

    // sv = [z0z2 + x0z0 + x1z2]^d, 0^{mv - d}
    for (j = 0; j < proof->rp.d; j++) {
        poly_set(&z0_ntt[j], &proof->z0[j]);
        poly_set(&z2_ntt[j], &proof->z2[j]);
        poly_ntt(&z0_ntt[j]);
        poly_ntt(&z2_ntt[j]);

        poly_pointwise_montgomery(&tmp1, &z0_ntt[j], &x0_ntt);
        poly_reduce(&tmp1);
        poly_pointwise_montgomery(&tmp2, &z2_ntt[j], &x1_ntt);
        poly_reduce(&tmp1);
        poly_add(&tmp1, &tmp1, &tmp2);
        poly_pointwise_montgomery(&sv[j], &z0_ntt[j],&z2_ntt[j]);
        poly_reduce(&sv[j]);
        poly_add(&sv[j], &sv[j], &tmp1); // z0z2 + x0z0 + x1z2 in NTT
        poly_inv_ntt_to_mont(&sv[j]);
        poly_reduce(&sv[j]);

        if (poly_chknorm(&sv[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Large norm (div signed) sv_%d\n", j));
            return -1;
        }
    }
    for (j = proof->rp.d; j < mv; j++) poly_set_zero(&sv[j], 0, N);

    commit(ctx, &t2, proof->s, sv, nullptr);

    poly_pointwise_montgomery(&x22_ntt, &x2_ntt, &x2_ntt);
    poly_inv_ntt_to_mont(&x22_ntt);
    poly_ntt(&x22_ntt);
    poly_n_sub(&tmp, u1, u3); // u1 - u3
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x22_ntt); // x2^2(u1 - u3) in NTT
    poly_n_inv_ntt_to_mont(&tmp);

    poly_n_set(&tmpn1, u0);
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x0_ntt); // x0u0 in NTT

    poly_n_set(&tmpn2, u2);
    poly_n_ntt(&tmpn2);
    poly_n_pointwise_montgomery(&tmpn2, &tmpn2, &x1_ntt); // x1u2 in NTT

    poly_n_add(&tmpn1, &tmpn1, &tmpn2); // x0u0 + x1u2 in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_reduce(&tmpn1);

    poly_n_roundup(&tmpn2, &proof->t1, t1_ERROR);
    poly_n_add(&tmpn1, &tmpn1, &tmpn2);  // t1 + x0u0 + x1u2
    poly_n_ntt(&tmpn1);
    poly_n_pointwise_montgomery(&tmpn1, &tmpn1, &x2_ntt); // x2(t1 + x0u0 + x1u2) in NTT
    poly_n_inv_ntt_to_mont(&tmpn1);
    poly_n_add(&tmp, &tmp, &tmpn1); // x2^2(u1 - u3) + x2(t1 + x0u0 + x1u2) in NTT
    poly_n_reduce(&tmp);

    poly_n_sub(&t2, &t2, &tmp);
    poly_n_reduce(&t2);

    poly_n_highbits(&t2, &t2, aggr_ERROR);
    poly_n_makeup(&t2, &proof->t2_hints);

    pack_poly_ring_custom(t2_bytes, &t2, K1 - aggr_ERROR);
    shake256_init(&state);
    shake256_absorb(&state, x1_bytes, SEED_BYTES);
    shake256_absorb(&state, t2_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x2_bytes, SEED_BYTES, &state);

    if (memcmp(x2_bytes, proof->x2, SEED_BYTES) == 0)
        return 0;

    return -1;
}

