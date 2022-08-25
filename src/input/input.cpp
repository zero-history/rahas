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

#include "input.h"

void tobinL(poly *b, zint_t v) {
    uint16_t j;
    for (j = 0; j < N; j++)
        b->coef[j] = (v >> j) &  1;
}

#define mr_start (mk + mv)
/**
 * Commits H(k | v | r) into u
 * @param ctx - context object
 * @param u - commitment (not NTT and reduced)
 * @param v - value vector (not NTT)
 * @param k - key vector (not NTT)
 * @param r - key vector (not NTT)
 */
void commit(context_t *ctx, poly_n *u, poly k[mk], poly v[mv], poly r[mr]) {
    int j, i;
    poly t;

    poly_n_set_zero(u, 0, N);
    for (i = 0; i < n; i++) {
        if (k != nullptr) {
            for (j = 0; j < mk; j++) {
                poly_set(&t, &k[j]);
                poly_ntt(&t);
                poly_pointwise_montgomery(&t, &ctx->H[i].vec[j], &t);
                poly_add(&u->vec[i], &u->vec[i], &t);
                poly_reduce(&u->vec[i]);
            }
        }
        if (v != nullptr) {
            for (j = 0; j < mv; j++) {
                poly_set(&t, &v[j]);
                poly_ntt(&t);
                poly_pointwise_montgomery(&t, &ctx->H[i].vec[j + mk], &t);
                poly_add(&u->vec[i], &u->vec[i], &t);
                poly_reduce(&u->vec[i]);

            }
        }
        if (r != nullptr) {
            for (j = 0; j < mr; j++) {
                poly_set(&t, &r[j]);
                poly_ntt(&t);
                poly_pointwise_montgomery(&t, &ctx->H[i].vec[j + mr_start], &t);
                poly_add(&u->vec[i], &u->vec[i], &t);
                poly_reduce(&u->vec[i]);

            }
        }
    }
    poly_n_inv_ntt_to_mont(u);
    poly_n_reduce(u);
}


void commit_from_int(context_t *ctx, uint8_t d, poly_n *u, poly k[mk], zint_t v[mv]) {
    int j, i;
    poly t;

    poly_n_set_zero(u, 0, N);
    for (i = 0; i < n; i++) {
        if (k != nullptr) {
            for (j = 0; j < mk; j++) {
                poly_set(&t, &k[j]);
                poly_ntt(&t);
                poly_pointwise_montgomery(&t, &ctx->H[i].vec[j], &t);
                poly_add(&u->vec[i], &u->vec[i], &t);
                poly_reduce(&u->vec[i]);
            }
        }
        if (v != nullptr) {
            for (j = 0; j < mv; j++) {
                poly_set_zero(&t, 0, N);
                if (j < d)
                    t.coef[0] = v[j];
                poly_ntt(&t);
                poly_pointwise_montgomery(&t, &ctx->H[i].vec[j + mk], &t);
                poly_add(&u->vec[i], &u->vec[i], &t);
                poly_reduce(&u->vec[i]);

            }
        }
    }
    poly_n_inv_ntt_to_mont(u);
    poly_n_reduce(u);
}

/**
 * Open a commitment and check their norms
 * @param ctx - context object
 * @param u - commitment
 * @param v - value vector
 * @param k - key vector
 * @return 0 - valid; (-1) invalid
 */
int open(context_t *ctx, poly_n *u, poly v[mv], poly k[mk]) {
    poly_n u1;
    int i;

    for (i = 0; i < mk; i++)
        if (poly_chknorm(&k[i], GAMMA1) != 0) {
            return -1;
        }

    for (i = 0; i < mv; i++)
        if (poly_chknorm(&v[i], GAMMA1) != 0) {
            return -1;
        }
    commit(ctx, &u1, k, v, nullptr);
    return poly_n_compare(&u1, u);
}


/**
 * Allocate memory for a range proof
 * @param rp - range proof
 * @param L1 - range size
 * @param d - number of integers
 */
int allocate_rp(rp_t *rp, uint8_t L1, uint8_t d) {
    int j;
    if (d > mv)
        return -1;
    rp->z = (poly **) malloc(d * sizeof(poly *));
    for (j = 0; j < d; j++) {
        rp->z[j] = (poly *) malloc(L1 * sizeof(poly));
    }
    return 0;
}


/**
 * Deallocate memory of a range proof
 * @param rp
 */
void deallocate_rp(rp_t *rp) {
    int j;
    for (j = 0; j < rp->d; ++j)
        free(rp->z[j]);
    free(rp->z);
}


/**
 * Generates a range proof (does not check whether values are valid or not)
 * @param ctx  - context object
 * @param L1  - range bit
 * @param d - size of v
 * @param u - commitment
 * @param v - value in plain text
 * @param k - secret key of the commitment
 * @return
 */
void range_gen(context_t *ctx, uint8_t L1, uint8_t d, rp_t *rp, poly_n *u, const zint_t *v, poly k[mk]) {
    int j, i;
    poly b[d];
    poly k1[mk];
    poly k2[mk];
    uint8_t k1_seed[mk][r1_BYTES];
    uint8_t k2_seed[mk][r2_BYTES];

    keccak_state state;
    poly x1;
    poly x1_ntt;
    poly x2;
    poly x2_ntt;
    uint8_t u_bytes[u_chal_HIGHBITS];
    uint8_t t1_bytes[t1_HIGHBITS];
    uint8_t t2_bytes[aggr_HIGHBITS];
    uint8_t x1_bytes[SEED_BYTES];

    poly tmp1;
    poly tmp2;
    poly tmp3;
    poly a[d][L1];
    poly a_ntt[d][L1];
    uint8_t a_seed[d][L1][a_BYTES];

    poly_n t2;
    poly_n t2_recreated;
    poly_n tmp;

    // Binary values
    for (j = 0; j < d; j++)
        tobinL(&b[j], v[j]);

    rp->L1 = L1;
    rp->d = d;

    rejection_point:

    // Generate keys
    for (j = 0; j < mk; j++) {
        get_mask_tau1(&k1[j], k1_seed[j]);
        get_mask_tau2(&k2[j], k2_seed[j]);
    }

    // Lazy sampling - a
    for (j = 0; j < d; j++) {
        for (i = 0; i < L1; i++) {
            get_alpha_mask(&a[j][i], a_seed[j][i], b[j].coef[i]);
            poly_set(&a_ntt[j][i], &a[j][i]);
            poly_ntt(&a_ntt[j][i]);
        }
    }


    // sr = [sum (a[j][i] * rot(2b[j].coef[i] - 1, i)), 0^{mr - d}]
    poly sr1[mr];
    for (j = 0; j < mr; j++) poly_set_zero(&sr1[j], 0, N);
    for (j = 0; j < d; j++) {
        for (i = 0; i < L1; i++) {
            poly_easy_mul(&tmp1, &a[j][i], i, 1);
            poly_easy_mul(&tmp1, &tmp1, i, (zint_t) (2 * b[j].coef[i] - 1));
            poly_add(&sr1[j], &sr1[j], &tmp1); // not in NTT
        }
    }
    // t1
    commit(ctx, &rp->t1, k1, nullptr, sr1);
    poly_n_highbits(&rp->t1, &rp->t1, t1_ERROR);

    // packing
    poly_n_highbits(&tmp, u, u_chal_ERROR);
    pack_poly_ring_custom(u_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t1_bytes, &rp->t1, K1 - t1_ERROR);

    // Challenge 1
    shake256_init(&state);
    shake256_absorb(&state, &L1, 1);
    shake256_absorb(&state, u_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t1_bytes, t1_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x1_bytes, SEED_BYTES, &state);
    poly_challenge(&x1, x1_bytes);
    poly_set(&x1_ntt, &x1);
    poly_ntt(&x1_ntt);

    // sr  = [x1 [sum(a[j][i])^2]^d, 0^{mr - d}]
    poly sr2[mr];
    for (j = 0; j < mr; j++) poly_set_zero(&sr2[j], 0, N);
    for (j = 0; j < d; j++) {
        for (i = 0; i < L1; i++) {
            poly_easy_mul(&tmp1, &a[j][i], i, 1);
            poly_ntt(&tmp1);
            poly_pointwise_montgomery(&tmp1, &tmp1, &tmp1);
            poly_add(&sr2[j], &sr2[j], &tmp1);
            poly_reduce(&sr2[j]);
        }
        poly_inv_ntt_to_mont(&sr2[j]);
        poly_ntt(&sr2[j]);
        poly_pointwise_montgomery(&sr2[j], &sr2[j], &x1_ntt); // x1 * sum a[i]a[i] (in NTT)
        poly_inv_ntt_to_mont(&sr2[j]); // x1 * sum a[i]a[i] (not in NTT)
        poly_reduce_exact(&sr2[j]);

        /*if (poly_chknorm(&sr2[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Rejected rp: sr2_%d\n", j);
            goto rejection_point;
        }*/
    }
    // sv = [[sum (2^ia[j][i])]^d, 0^{mv - d}]
    poly sv1[mv];
    for (j = 0; j < mv; j++) poly_set_zero(&sv1[j], 0, N);
    for (j = 0; j < d; j++) {
        for (i = 0; i < L1; i++) {
            poly_easy_mul(&tmp1, &a[j][i], 0, (zint_t) 1 << i);
            poly_add(&sv1[j], &sv1[j], &tmp1); // not in NTT
        }
        /*if (poly_chknorm(&sv1[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Rejected rp: sv1_%d\n", j);
            goto rejection_point;
        }*/
    }

    // t2
    commit(ctx, &t2, k2, sv1, sr2);
    poly_n_highbits(&t2, &t2, aggr_ERROR);

    // packing
    pack_poly_ring_custom(t2_bytes, &t2, K1 - aggr_ERROR);

    // Challenge 2
    shake256_init(&state);
    shake256_absorb(&state, x1_bytes, SEED_BYTES);
    shake256_absorb(&state, t2_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(rp->x2, SEED_BYTES, &state);
    poly_challenge(&x2, rp->x2);
    poly_set(&x2_ntt, &x2);
    poly_ntt(&x2_ntt);

    // z_i = x2rot(b_i, i) + a_i
    for (j = 0; j < d; j++) {
        for (i = 0; i < L1; i++) {
            if (b[j].coef[i] == 0) {
                poly_set(&rp->z[j][i], &a[j][i]);
            } else {
                //poly_easy_mul(&tmp1, &x2, 0, 1);
                poly_add(&rp->z[j][i], &x2, &a[j][i]);
            }
            if (poly_chknorm(&rp->z[j][i], SR_RP_z) != 0) {
                DEBUG_PRINT(("Rejected z_%d %d\n", i, j));
                goto rejection_point;
            }
        }
    }

    // s
    for (j = 0; j < mk; j++) poly_set_zero(&rp->s[j], 0, N);
    for (j = 0; j < mk; j++) {
        poly_set(&rp->s[j], &k1[j]);
        poly_ntt(&rp->s[j]);
        poly_pointwise_montgomery(&rp->s[j], &rp->s[j], &x1_ntt);
        poly_inv_ntt_to_mont(&rp->s[j]);
        poly_reduce_exact(&rp->s[j]);
        poly_add(&rp->s[j], &rp->s[j], &k[j]);
        poly_ntt(&rp->s[j]);
        poly_pointwise_montgomery(&rp->s[j], &rp->s[j], &x2_ntt);
        poly_inv_ntt_to_mont(&rp->s[j]);
        poly_reduce_exact(&rp->s[j]);
        poly_add(&rp->s[j], &rp->s[j], &k2[j]);

        // TAU2 - CHI*CHI*TAU - CHI*TAU1
        if (poly_chknorm(&rp->s[j], SR_RP_s) != 0) {
            DEBUG_PRINT(("Rejected R_%d %d\n", j, poly_chknorm(&rp->s[j], SR_RP_s)));
            goto rejection_point;
        }
    }

    // Verify norms (sv2 = s'_1 and sr2 = s'_2)
    // sv = 2^i(sum z)^L1
    poly sv2[d];
    for (j = 0; j < d; j++) poly_set_zero(&sv2[j], 0, N);
    for (j = 0; j < rp->d; j++) {
        for (i = 0; i < rp->L1; i++) {
            poly_easy_mul(&tmp1, &rp->z[j][i], 0, (zint_t) 1 << i);
            poly_add(&sv2[j], &sv2[j], &tmp1);
        }

        if (poly_chknorm(&sv2[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Rejected (rp) s'_1: sv2_%d\n", j));
            goto rejection_point;
        }
    }

    // sr  = [x1 [sum(a[j][i])^2]^d]
    poly sr3[d];
    for (j = 0; j < d; j++) poly_set_zero(&sr3[j], 0, N);
    for (j = 0; j < rp->d; j++) {
        for (i = 0; i < rp->L1; i++) {
            poly_easy_mul(&tmp2, &rp->z[j][i], i, 1);
            poly_ntt(&tmp2);
            poly_easy_mul(&tmp3, &x2, i, 1);
            poly_ntt(&tmp3);
            poly_sub(&tmp3, &tmp2, &tmp3);
            poly_pointwise_montgomery(&tmp2, &tmp2, &tmp3); // z(z - x2) in NTT
            poly_add(&sr3[j], &sr3[j], &tmp2);
            poly_reduce(&sr3[j]);
        }
        poly_inv_ntt_to_mont(&sr3[j]);
        poly_ntt(&sr3[j]);
        poly_pointwise_montgomery(&sr3[j], &sr3[j], &x1_ntt); // x1 * sum a[i]a[i] (in NTT)
        poly_inv_ntt_to_mont(&sr3[j]); // x1 * sum a[i]a[i] (not in NTT)
        poly_reduce_exact(&sr3[j]);

        if (poly_chknorm(&sr3[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Rejected (rp) s'_2: sr3_%d\n", j));
            goto rejection_point;
        }
    }

    // t2 (recreate)
    commit(ctx, &t2_recreated, rp->s, sv2, sr3);

    poly_n_set(&tmp, &rp->t1);
    poly_n_roundup(&tmp, &tmp, t1_ERROR);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x1_ntt);
    poly_n_inv_ntt_to_mont(&tmp);
    poly_n_add(&tmp, &tmp, u);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x2_ntt);
    poly_n_inv_ntt_to_mont(&tmp);
    poly_n_sub(&t2_recreated, &t2_recreated, &tmp);
    poly_n_reduce(&t2_recreated);
    poly_n_highbits(&t2_recreated, &t2_recreated, aggr_ERROR);

    int res = poly_n_hints(&rp->t2_hints, &t2_recreated, &t2);
    if (res == -1) {
        DEBUG_PRINT(("Rejected t2_hints (rp)\n"));
        goto rejection_point;
    }

    poly_n_makeup(&t2_recreated, &rp->t2_hints);
    if (poly_n_compare(&t2_recreated, &t2) != 0) {
        DEBUG_PRINT(("Not equal t2 (rp)\n"));
    }

}

/**
 * Verifies a range proof of a commitment
 * @param ctx
 * @param u
 * @param rp
 * @return 0 - valid otherwise invalid
 */
int range_ver(context_t *ctx, poly_n *u, rp_t *rp) {
    keccak_state state;
    poly x1;
    poly x1_ntt;
    poly x2;
    poly x2_ntt;
    uint8_t u_bytes[u_chal_HIGHBITS];
    uint8_t t1_bytes[t1_HIGHBITS];
    uint8_t t2_bytes[aggr_HIGHBITS];
    uint8_t x1_bytes[SEED_BYTES];
    uint8_t x2_bytes[SEED_BYTES];

    poly tmp1;
    poly tmp2;
    poly tmp3;
    poly_n tmp;

    poly_n t2;
    int j, i;

    // Check s
    for (i = 0; i < mk; i++) {
        if (poly_chknorm(&rp->s[i], GAMMA1) != 0)
            return -1;
    }
    // Check norms
    for (j = 0; j < rp->d; j++) {
        for (i = 0; i < rp->L1; i++) {
            if (poly_chknorm(&rp->z[j][i], ALPHA) != 0)
                return -1;
        }
    }

    // packing
    poly_n_highbits(&tmp, u, u_chal_ERROR);
    pack_poly_ring_custom(u_bytes, &tmp, K1 - u_chal_ERROR);
    pack_poly_ring_custom(t1_bytes, &rp->t1, K1 - t1_ERROR);

    // Challenge 1
    shake256_init(&state);
    shake256_absorb(&state, &rp->L1, 1);
    shake256_absorb(&state, u_bytes, u_chal_HIGHBITS);
    shake256_absorb(&state, t1_bytes, t1_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x1_bytes, SEED_BYTES, &state);
    poly_challenge(&x1, x1_bytes);
    poly_set(&x1_ntt, &x1);
    poly_ntt(&x1_ntt);

    // Challenge 2
    poly_challenge(&x2, rp->x2);
    poly_set(&x2_ntt, &x2);
    poly_ntt(&x2_ntt);

    // sv = 2^i(sum z)^L1
    poly sv2[mv];
    for (j = 0; j < mv; j++) poly_set_zero(&sv2[j], 0, N);
    for (j = 0; j < rp->d; j++) {
        for (i = 0; i < rp->L1; i++) {
            poly_easy_mul(&tmp1, &rp->z[j][i], 0, (zint_t) 1 << i);
            poly_add(&sv2[j], &sv2[j], &tmp1);
        }

        if (poly_chknorm(&sv2[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Large norm (rp) s'_1: sv2_%d\n", j));
            return -1;
        }
    }

    // sr  = [x1 [sum(a[j][i])^2]^d, 0^{mr - d}]
    poly sr3[mr];
    for (j = 0; j < mr; j++) poly_set_zero(&sr3[j], 0, N);
    for (j = 0; j < rp->d; j++) {
        for (i = 0; i < rp->L1; i++) {
            poly_easy_mul(&tmp2, &rp->z[j][i], i, 1);
            poly_ntt(&tmp2);
            poly_easy_mul(&tmp3, &x2, i, 1);
            poly_ntt(&tmp3);
            poly_sub(&tmp3, &tmp2, &tmp3);
            poly_pointwise_montgomery(&tmp2, &tmp2, &tmp3); // z(z - x2) in NTT
            poly_add(&sr3[j], &sr3[j], &tmp2);
            poly_reduce(&sr3[j]);
        }
        poly_inv_ntt_to_mont(&sr3[j]);
        poly_ntt(&sr3[j]);
        poly_pointwise_montgomery(&sr3[j], &sr3[j], &x1_ntt); // x1 * sum a[i]a[i] (in NTT)
        poly_inv_ntt_to_mont(&sr3[j]); // x1 * sum a[i]a[i] (not in NTT)
        poly_reduce_exact(&sr3[j]);

        if (poly_chknorm(&sr3[j], GAMMA1) != 0) {
            DEBUG_PRINT(("Large norm (rp) s'_2: sr3_%d\n", j));
            return -1;
        }
    }

    // t2 (recreate)
    commit(ctx, &t2, rp->s, sv2, sr3);

    poly_n_set(&tmp, &rp->t1);
    poly_n_roundup(&tmp, &tmp, t1_ERROR);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x1_ntt);
    poly_n_inv_ntt_to_mont(&tmp);
    poly_n_add(&tmp, &tmp, u);
    poly_n_ntt(&tmp);
    poly_n_pointwise_montgomery(&tmp, &tmp, &x2_ntt);
    poly_n_inv_ntt_to_mont(&tmp);
    poly_n_sub(&t2, &t2, &tmp);
    poly_n_reduce(&t2);

    // Packing
    poly_n_highbits(&t2, &t2, aggr_ERROR);
    poly_n_makeup(&t2, &rp->t2_hints);
    pack_poly_ring_custom(t2_bytes, &t2, K1 - aggr_ERROR);

    // Challenge 2
    shake256_init(&state);
    shake256_absorb(&state, x1_bytes, SEED_BYTES);
    shake256_absorb(&state, t2_bytes, aggr_HIGHBITS);
    shake256_finalize(&state);
    shake256_squeeze(x2_bytes, SEED_BYTES, &state);

    if (memcmp(rp->x2, x2_bytes, SEED_BYTES) == 0)
        return 0;
    return -1;

}

/**
 * Generate a commitment and a range proof
 * @param ctx - context
 * @param L1 -
 * @param d
 * @param rp
 * @param u
 * @param v
 * @param k
 * @return
 */
int rcommit(context_t *ctx, uint8_t L1, uint8_t d, rp_t *rp, poly_n *u, zint_t v[], poly k[mk]){

    for (int i = 0; i < d; i++)
        if (v[i] < 0 || v[i] >= ((zint_t)1 << L1))
            return -1;
    commit_from_int(ctx, d, u, k, v);
    range_gen(ctx, L1, d, rp, u, v, k);

    return 0;
}


