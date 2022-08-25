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
 * Generate a proof if u1(v) is equal to u2(v)
 * @param ctx - context object
 * @param proof - zero-knowledge proof
 * @param u1 - input commitment
 * @param v1 - input vector
 * @param k1 - input key
 * @param u2 - input commitment
 * @param v2 - input vector
 * @param k2 - input key
 * @return 0 - passed, (-1) - failed
 */
int equal_gen_from_int(context_t *ctx, eq_pt *proof,
                       com_t *u1, zint_t *v1, mask_t *k1,
                       com_t *u2, zint_t *v2, mask_t *k2) {
    keccak_state state;
    poly x;
    poly x_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t t_bytes[aggr_HIGHBITS];

    poly_n t;
    poly_n t_recreated;
    poly_n tmp;

    poly k3[mk];
    uint8_t tau3_seed[r3_BYTES];
    int j;

    // Confirm equality
    for (j = 0; j < mv; j++) {
        if (v1[j] != v2[j]) {
            return -1;
        }
    }

    rejection_point:

    // Get masks
    for (j = 0; j < mk; j++){
        get_mask_tau3(&k3[j], tau3_seed);
    }

    // t
    commit(ctx, &t, k3, nullptr, nullptr);
    poly_n_highbits(&t, &t, aggr_ERROR);

    // Packing
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t_bytes, &t, K1 - aggr_ERROR);

    // Challenge
    shake256_init(&state);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(proof->x, SEED_BYTES, &state);
    poly_challenge(&x, proof->x);
    poly_set(&x_ntt, &x);
    poly_ntt(&x_ntt);

    for (j = 0; j < mk; j++) {
        poly_sub(&proof->s[j], &k1->k[j], &k2->k[j]);
        poly_ntt(&proof->s[j]);
        poly_pointwise_montgomery(&proof->s[j], &proof->s[j], &x_ntt);
        poly_inv_ntt_to_mont(&proof->s[j]);
        poly_add(&proof->s[j], &proof->s[j], &k3[j]);
        poly_reduce(&proof->s[j]);

        if (poly_chknorm(&proof->s[j], SR_EQ_s) != 0) {
            DEBUG_PRINT(("Rejected eq s_%d\n", j));
            goto rejection_point;
        }
    }

    // Create Hints
    commit(ctx, &t_recreated, proof->s, nullptr, nullptr);

    poly_n_sub(&tmp, u1, u2);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x_ntt);
    poly_n_inv_ntt_to_mont(&tmp);
    poly_n_sub(&t_recreated, &t_recreated, &tmp);
    poly_n_reduce(&t_recreated);
    poly_n_highbits(&t_recreated, &t_recreated, aggr_ERROR);

    int res = poly_n_hints(&proof->t_hints, &t_recreated, &t);
    if (res == -1) {
        DEBUG_PRINT(("Rejected t2_hints (signed div)\n"));
        goto rejection_point;
    }

    poly_n_makeup(&t_recreated, &proof->t_hints);
    if (poly_n_compare(&t_recreated, &t) != 0) {
        DEBUG_PRINT(("Not equal (signed div)\n"));
    }

    return 0;
}

/**
 * Generate a proof if u1(v) is equal to u2(v)
 * @param ctx - context object
 * @param proof - zero-knowledge proof
 * @param u1 - input commitment
 * @param v1 - input vector
 * @param k1 - input key
 * @param u2 - input commitment
 * @param v2 - input vector
 * @param k2 - input key
 * @return 0 - passed, (-1) - failed
 */
int equal_gen(context_t *ctx, eq_pt *proof,
              com_t *u1, poly v1[mv], mask_t *k1,
              com_t *u2, poly v2[mv], mask_t *k2) {

    keccak_state state;
    poly x;
    poly x_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t t_bytes[aggr_HIGHBITS];

    poly_n t;
    poly_n t_recreated;
    poly_n tmp;

    poly k3[mk];
    uint8_t tau3_seed[r3_BYTES];
    int j;

    // Confirm equality
    for (j = 0; j < mv; j++) {
        if (poly_compare(&v1[j], &v2[j]) != 0) {
            return -1;
        }
    }

    rejection_point:

    // Get masks
    for (j = 0; j < mk; j++){
        get_mask_tau3(&k3[j], tau3_seed);
    }

    // t
    commit(ctx, &t, k3, nullptr, nullptr);
    poly_n_highbits(&t, &t, aggr_ERROR);

    // Packing
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t_bytes, &t, K1 - aggr_ERROR);

    // Challenge
    shake256_init(&state);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(proof->x, SEED_BYTES, &state);
    poly_challenge(&x, proof->x);
    poly_set(&x_ntt, &x);
    poly_ntt(&x_ntt);

    for (j = 0; j < mk; j++) {
        poly_sub(&proof->s[j], &k1->k[j], &k2->k[j]);
        poly_ntt(&proof->s[j]);
        poly_pointwise_montgomery(&proof->s[j], &proof->s[j], &x_ntt);
        poly_inv_ntt_to_mont(&proof->s[j]);
        poly_add(&proof->s[j], &proof->s[j], &k3[j]);
        poly_reduce(&proof->s[j]);

        if (poly_chknorm(&proof->s[j], SR_EQ_s) != 0) {
            DEBUG_PRINT(("Rejected eq s_%d\n", j));
            goto rejection_point;
        }
    }

    // Create Hints
    commit(ctx, &t_recreated, proof->s, nullptr, nullptr);

    poly_n_sub(&tmp, u1, u2);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x_ntt);
    poly_n_inv_ntt_to_mont(&tmp);
    poly_n_sub(&t_recreated, &t_recreated, &tmp);
    poly_n_reduce(&t_recreated);
    poly_n_highbits(&t_recreated, &t_recreated, aggr_ERROR);

    int res = poly_n_hints(&proof->t_hints, &t_recreated, &t);
    if (res == -1) {
        DEBUG_PRINT(("Rejected t2_hints (signed div)\n"));
        goto rejection_point;
    }

    poly_n_makeup(&t_recreated, &proof->t_hints);
    if (poly_n_compare(&t_recreated, &t) != 0) {
        DEBUG_PRINT(("Not equal (signed div)\n"));
    }

    return 0;
}


/**
 * Verify equality
 * @param ctx - context
 * @param u1 - input 1
 * @param u2 - input 2
 * @param proof - zero-knowledge proof
 * @return 0 - valid, (-1) otherwise
 */
int equal_ver(context_t *ctx, com_t *u1, com_t *u2, eq_pt *proof) {
    keccak_state state;
    poly x;
    poly x_ntt;
    uint8_t u1_bytes[u_chal_HIGHBITS];
    uint8_t u2_bytes[u_chal_HIGHBITS];
    uint8_t t_bytes[aggr_HIGHBITS];
    uint8_t x_bytes[SEED_BYTES];

    poly_n t;
    poly_n tmp;
    int j;

    // Get masks
    for (j = 0; j < mk; j++){
        if (poly_chknorm(&proof->s[j], TAU3) != 0) {
            return -1;
        }
    }

    poly_challenge(&x, proof->x);
    poly_set(&x_ntt, &x);
    poly_ntt(&x_ntt);

    commit(ctx, &t, proof->s, nullptr, nullptr);

    poly_n_sub(&tmp, u1, u2);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x_ntt);
    poly_n_inv_ntt_to_mont(&tmp);
    poly_n_sub(&t, &t, &tmp);
    poly_n_reduce(&t);

    poly_n_highbits(&t, &t, aggr_ERROR);
    poly_n_makeup(&t, &proof->t_hints);

    // Packing
    poly_n_highbits(&tmp, u1, u_chal_ERROR);
    pack_poly_ring_custom(u1_bytes, &tmp, K1 - u_chal_ERROR);
    poly_n_highbits(&tmp, u2, u_chal_ERROR);
    pack_poly_ring_custom(u2_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t_bytes, &t, K1 - aggr_ERROR);

    // Recreate the challenge
    shake256_init(&state);
    shake256_absorb(&state, u1_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, u2_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x_bytes, SEED_BYTES, &state);

    if (memcmp(x_bytes, proof->x, SEED_BYTES) == 0)
        return 0;

    return -1;
}


/**
 * Allocate memory for a ''not equal'' proof
 * @param proof - proof
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - number of integers
 */
int allocate_neq_proof(neq_pt *proof, uint8_t L1, uint8_t d) {
    if (d > mv)
        return -1;
    allocate_mul_proof(&proof->proof, L1, d);
    // 2(L1 + 1) bits because we don't want to disclose that their multiplication is in range L1
    allocate_rp(&proof->rp, 2*(L1 + 1), d);
    return 0;
}


/**
 * Deallocate memory of a ''not equal'' proof
 * @param proof
 */
void deallocate_neq_proof(neq_pt *proof) {
    deallocate_mul_proof(&proof->proof);
    deallocate_rp(&proof->rp);
}


/**
 * Creates a proof for not equal statement; u1(v) != u2(v)
 * @param ctx - context object
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - value size
 * @param proof - zero-knowledge proof
 * @param u1 - input commitment
 * @param v1 - input vector
 * @param k1 - input key
 * @param u2 - input commitment
 * @param v2 - input vector
 * @param k2 - input key
 * @return 0 - passed, (-1) - failed
 */
int nequal_gen(context_t *ctx, uint8_t L1, const uint8_t d, neq_pt *proof,
               com_t *u1, zint_t *v1, mask_t *k1,
               com_t *u2, zint_t *v2, mask_t *k2) {

    poly_n u11;
    poly_n u21;
    mask_t k11;
    mask_t k21;
    mask_t k0;
    zint_t v11[d];
    zint_t v21[d];
    zint_t v0[d];
    poly_n e;
    zint_t one[d];
    int j;

    // Confirm equality for each value[j]
    for (j = 0; j < d; j++) {
        if (v1[j] == v2[j]) {
            return -1;;
        }
    }

    // e
    for (j = 0; j < d; j++) one[j] = 1;
    commit_from_int(ctx, d, &e, nullptr, one);

    for (j = 0; j < d; j++) {
        v11[j] = -(v1[j] - (v2[j] + 1));
        v21[j] = (v2[j] - 1) - v1[j];
    }

    for (j = 0; j < mk; j++) {
        poly_sub(&k11.k[j], &k2->k[j], &k1->k[j]);
        poly_set(&k21.k[j], &k11.k[j]);
    }

    poly_n_add(&u11, u2, &e);
    poly_n_sub(&u11, &u11, u1);
    poly_n_reduce(&u11);

    poly_n_sub(&u21, u2, &e);
    poly_n_sub(&u21, &u21, u1);
    poly_n_reduce(&u21);

    mul_gen(ctx, 2*(L1 + 1), d, &proof->proof,
            &proof->u, v0, &k0,
            &u11, v11, &k11, &u21, v21, &k21);
    // 2(L1 + 1) bits because we don't want to disclose that their multiplication is in range L1
    range_gen(ctx, 2*(L1 + 1), d, &proof->rp, &proof->u, v0, k0.k);
    return 0;
}

/**
 * Verifies a ''not equal'' statement; u1(v) ?!= u2(v)
 * @param ctx - context object
 * @param u1 - input 1
 * @param u2 - input 2
 * @param proof - zero-knowledge proof
 * @return 0 - valid, (-1) - invalid
 */
int nequal_ver(context_t *ctx, com_t *u1, com_t *u2, neq_pt *proof) {
    poly_n u11;
    poly_n u21;
    poly_n e;
    int j;
    zint_t one[proof->rp.d];

    // e
    for (j = 0; j < proof->rp.d; j++) one[j] = 1;
    commit_from_int(ctx, proof->rp.d, &e, nullptr, one);

    poly_n_add(&u11, u2, &e);
    poly_n_sub(&u11, &u11, u1);
    poly_n_reduce(&u11);

    poly_n_sub(&u21, u2, &e);
    poly_n_sub(&u21, &u21, u1);
    poly_n_reduce(&u21);

    if (mul_ver(ctx, &proof->u, &u11, &u21, &proof->proof) == 0
        && range_ver(ctx, &proof->u, &proof->rp) == 0)
        return 0;

    return -1;

}

/**
 * Allocate memory for a ''leq'' proof
 * @param rp -  proof
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - number of integers
 */
int allocate_leq(leq_proof_t *proof, uint8_t L1, uint8_t d) {
    return allocate_rp(proof, L1, d);
}


/**
 * Deallocate memory of a "leq" proof
 * @param rp
 */
void deallocate_leq(leq_proof_t *proof) {
    return deallocate_rp(proof);
}

/**
 * Creates a proof for less or equal statement; u1(v) <= u2(v)
 * @param ctx - context object
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - value size
 * @param proof - zero-knowledge proof
 * @param u1 - input commitment
 * @param v1 - input vector
 * @param k1 - input key
 * @param u2 - input commitment
 * @param v2 - input vector
 * @param k2 - input key
 * @return 0 - passed, (-1) - failed
 */
int leq_gen(context_t *ctx, uint8_t L1, uint8_t d, leq_proof_t *proof,
            com_t *u1, zint_t *v1, mask_t *k1,
            com_t *u2, zint_t *v2, mask_t *k2) {
    poly_n u;
    mask_t k;
    zint_t v[d];
    int j;

    // Confirm equality for each value[j]
    for (j = 0; j < d; j++) {
        if (v1[j] > v2[j]) {
            return -1;;
        }
    }

    for (j = 0; j < d; j++) {
        v[j] = v2[j] - v1[j];
    }

    for (j = 0; j < mk; j++) {
        poly_sub(&k.k[j], &k2->k[j], &k1->k[j]);
    }

    poly_n_sub(&u, u2, u1);
    poly_n_reduce(&u);

    range_gen(ctx, L1, d, proof, &u, v, k.k);

    return 0;
}


/**
 * Verifies "leq" statement u1(v) <= u2(v)
 * @param ctx - context object
 * @param u1 - input 1
 * @param u2 - input 2
 * @param proof - zero-knowledge proof
 * @return 0 - valid, (-1) - invalid
 */
int leq_ver(context_t *ctx, com_t *u1, com_t *u2, leq_proof_t *proof) {
    poly_n u;
    poly_n_sub(&u, u2, u1);
    poly_n_reduce(&u);

    return range_ver(ctx, &u, proof);

}


/**
 * Allocate memory for a ''leq'' proof
 * @param rp -  proof
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - number of integers
 */
int allocate_geq(leq_proof_t *proof, uint8_t L1, uint8_t d) {
    return allocate_rp(proof, L1, d);
}


/**
 * Deallocate memory of a "leq" proof
 * @param rp
 */
void deallocate_geq(leq_proof_t *proof) {
    return deallocate_rp(proof);
}

/**
 * Creates a proof for greater or equal statement; u1(v) >= u2(v)
 * @param ctx - context object
 * @param L1 - range size is -(2^{L1} - 1), (2^{L1} - 1),
 * @param d - value size
 * @param proof - zero-knowledge proof
 * @param u1 - input commitment
 * @param v1 - input vector
 * @param k1 - input key
 * @param u2 - input commitment
 * @param v2 - input vector
 * @param k2 - input key
 * @return 0 - passed, (-1) - failed
 */
int geq_gen(context_t *ctx, uint8_t L1, uint8_t d, geq_proof_t *proof,
            com_t *u1, zint_t *v1, mask_t *k1,
            com_t *u2, zint_t *v2, mask_t *k2) {
    poly_n u;
    mask_t k;
    zint_t v[d];
    int j;

    // Confirm equality for each value[j]
    for (j = 0; j < d; j++) {
        if (v1[j] < v2[j]) {
            return -1;;
        }
    }

    for (j = 0; j < d; j++) {
        v[j] = v1[j] - v2[j];
    }

    for (j = 0; j < mk; j++) {
        poly_sub(&k.k[j], &k1->k[j], &k2->k[j]);
    }

    poly_n_sub(&u, u1, u2);
    poly_n_reduce(&u);

    range_gen(ctx, L1, d, proof, &u, v, k.k);

    return 0;
}


/**
 * Verifies "leq" statement u1(v) <= u2(v)
 * @param ctx - context object
 * @param u1 - input 1
 * @param u2 - input 2
 * @param proof - zero-knowledge proof
 * @return 0 - valid, (-1) - invalid
 */
int geq_ver(context_t *ctx, com_t *u1, com_t *u2, geq_proof_t *proof) {
    poly_n u;
    poly_n_sub(&u, u1, u2);
    poly_n_reduce(&u);

    return range_ver(ctx, &u, proof);

}

