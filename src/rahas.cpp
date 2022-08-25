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

#ifndef ZKSC_LIBRARY
#define ZKSC_LIBRARY


#include <utility>

#include "rahas.h"

/**
 * Initialize the context object that holds the matrix
 * @param seed - the matrix will be generated from this
 * @return the context object
 */
context_t init(uint8_t seed[SEED_BYTES]) {
    context_t ctx;
    shake256(ctx.seed, SEED_BYTES, seed, SEED_BYTES);
    poly_matrix_expand(ctx.H, ctx.seed); // in NTT domain

    poly_set_zero(&ctx.one_N, 0, N);
    ctx.one_N.coef[N - 1] = -1;
    poly_ntt(&ctx.one_N);
    return ctx;
}

void com_set_zero(com_t *c) {
    poly_n_set_zero(c, 0, N);
}

/**
 * Set b to c
 * @param c - out
 * @param b - in
 */
void com_set(com_t *c, com_t *b) {
    poly_n_set(c, b);
    poly_n_lowbit_clean(c, c, u_ERROR);
}

/**
 * Add commitments
 * @param c - a + b
 * @param a - input
 * @param b - input
 */
void com_add(com_t *c, com_t *a, com_t *b) {
    poly_n_add(c, a, b);
    poly_n_reduce(c);
    poly_n_lowbit_clean(c, c, u_ERROR);
}

/**
 * Suntract Commitments
 * @param c - a - b
 * @param a - inout
 * @param b - inout
 */
void com_sub(com_t *c, com_t *a, com_t *b) {
    poly_n_sub(c, a, b);
    poly_n_reduce(c);
    poly_n_lowbit_clean(c, c, u_ERROR);
}

/**
 * Compare two commitments
 * @param c
 * @param b
 * @return 0 - equal, (-1) - different
 */
int com_compare(com_t *c, com_t *b) {
    poly_n tmp1;
    poly_n tmp2;
    poly_n_lowbit_clean(&tmp1, c, u_chal_ERROR);
    poly_n_lowbit_clean(&tmp2, b, u_chal_ERROR);
    return poly_n_compare(&tmp1, &tmp2);
}


/**
 * Generate input object structure for compiling (includes the secret key and the value)
 * @param ctx - context object
 * @param out - compiling input object
 * @param v - value (confidential)
 * @param key - key (confidential)
 * @param is_set_pk - is their any public identifier
 * @param pk - public identifier
 */
void in_ct_gen(context_t *ctx, in_ct *out, zint_t v, uint8_t key[MASKKEY_BYTES],
               uint8_t is_set_pk, uint8_t pk[PKID]) {
    int i;

    // Set the key
    for (i = 0; i < mk; i++)
        set_mask_tau(&out->k.k[i], key + i * r_BYTES);

    // Set the value
    out->v = std::move(v);

    commit_from_int(ctx, 1, &out->u, out->k.k, &out->v);

    out->is_set_pk = is_set_pk;
    if (is_set_pk == 1) {
        memcpy(out->pk, pk, PKID);
    }
}

/**
 * Copy a compiling input object
 * @param out - output
 * @param in  - input
 */
void in_ct_copy(in_ct *out, in_ct *in) {
    out->v = in->v;
    com_set(&out->u, &in->u);
    for (int i = 0; i < mk; i++) {
        poly_set(&out->k.k[i], &in->k.k[i]);
    }
    out->is_set_pk = in->is_set_pk;
    if (in->is_set_pk == 1)
        memcpy(out->pk, in->pk, PKID);
}

/**
 * Copy input object structure for execution
 * @param out
 * @param in
 */
void in_et_copy(in_et *out, in_et *in) {
    out->v = in->v;
    com_set(&out->u, &in->u);
    out->is_set_pk = in->is_set_pk;
    if (in->is_set_pk == 1)
        memcpy(out->pk, in->pk, PKID);
}

/**
 * Copy compiling input object to an execution input object
 * @param out - execution input object
 * @param in - compiling input object
 */
void in_ct_to_et(in_et *out, in_ct *in) {
    out->v = in->v;
    com_set(&out->u, &in->u);
    out->is_set_pk = in->is_set_pk;
    if (in->is_set_pk == 1)
        memcpy(out->pk, in->pk, PKID);
}

/**
 * Initiate a compiling coin
 * @param coin - output
 * @param range - range bit
 */
void coin_ct_init(coin_ct *coin, uint8_t range) {
    allocate_rp(&coin->rp, range, 1);
}

/**
 * Free a compiling coin
 * @param coin
 */
void coin_ct_free(coin_ct *coin) {
    deallocate_rp(&coin->rp);
}

/**
 * Initiate a executing coin
 * @param coin - output
 * @param range - range bit
 */
void coin_et_init(coin_et *coin, uint8_t range) {
    allocate_rp(&coin->rp, range, 1);
}

/**
 * Free a executing coin
 * @param coin
 */
void coin_et_free(coin_et *coin) {
    deallocate_rp(&coin->rp);
}

/**
 * Generate compiling coin (+ secret key and value)
 * @param ctx - context object
 * @param range - range bit
 * @param coin - output
 * @param v - coin amount
 * @param key - key
 * @return
 */
int coin_gen(context_t *ctx, uint8_t range, coin_ct *coin, uint32_t v, uint8_t key[MASKKEY_BYTES]) {
    int i;

    // Set the key
    for (i = 0; i < mk; i++)
        set_mask_tau(&coin->c.k.k[i], key + i * r_BYTES);

    // Set the value
    coin->c.v = v;

    // Generate the commitment
    return rcommit(ctx, range, 1, &coin->rp, &coin->c.u, &coin->c.v, coin->c.k.k);
}

void coin_ct_to_et(coin_et *out, coin_ct *in) {
    com_set(&out->c.u, &in->c.u);
    out->c.is_set_pk = in->c.is_set_pk;
    if (in->c.is_set_pk == 1)
        memcpy(out->c.pk, in->c.pk, PKID);
    copy_rp(&out->rp, &in->rp);
}

int coin_ver(context_t *ctx, coin_et *coin) {
    return range_ver(ctx, &coin->c.u, &coin->rp);
}

int get_coin_size(coin_ct *coin) {
    return (COM_BYTES + get_rp_size(&coin->rp));
}

int get_coin_size_et(coin_et *coin) {
    return (COM_BYTES + get_rp_size(&coin->rp));
}

void coin_to_bytes(uint8_t bytes[], coin_ct *coin) {
    com_to_bytes(bytes, &coin->c.u);
    rp_to_bytes(bytes + COM_BYTES, &coin->rp);
}

void coin_to_bytes_et(uint8_t bytes[], coin_et *coin) {
    com_to_bytes(bytes, &coin->c.u);
    rp_to_bytes(bytes + COM_BYTES, &coin->rp);
}

void bytes_to_coin(coin_et *coin, uint8_t bytes[]) {
    bytes_to_com(&coin->c.u, bytes);
    bytes_to_rp(&coin->rp, bytes + COM_BYTES);
}

void bytes_to_coin_ct(coin_ct *coin, uint8_t bytes[]) { // no keys
    bytes_to_com(&coin->c.u, bytes);
    bytes_to_rp(&coin->rp, bytes + COM_BYTES);
}

void coin_ct_c_set_int(context_t *ctx, coin_ct *c, zint_t v) {
    if (v == 0)
        commit_from_int(ctx, 1, &c->c.u, nullptr, nullptr);
    else
        commit_from_int(ctx, 1, &c->c.u, nullptr, &v);
    for (int i = 0; i < mk; i++)
        poly_set_zero(&c->c.k.k[i], 0, N);
    c->c.v = v;
}

void coin_ct_c_set(coin_ct *c, coin_ct *b) {
    com_set(&c->c.u, &b->c.u);
    for (int i = 0; i < mk; i++)
        poly_set(&c->c.k.k[i], &b->c.k.k[i]);
    c->c.v = b->c.v;
}

void coin_ct_add(coin_ct *c, coin_ct *a, coin_ct *b) {
    com_add(&c->c.u, &a->c.u, &b->c.u);
    for (int i = 0; i < mk; i++)
        poly_add(&c->c.k.k[i], &a->c.k.k[i], &b->c.k.k[i]);
    c->c.v = a->c.v + b->c.v;
}

void coin_ct_sub(coin_ct *c, coin_ct *a, coin_ct *b) {
    com_sub(&c->c.u, &a->c.u, &b->c.u);\
    for (int i = 0; i < mk; i++)
        poly_sub(&c->c.k.k[i], &a->c.k.k[i], &b->c.k.k[i]);
    c->c.v = a->c.v - b->c.v;
}


void coin_et_add(coin_et *c, coin_et *a, coin_et *b) {
    com_add(&c->c.u, &a->c.u, &b->c.u);
}


void coin_et_sub(coin_et *c, coin_et *a, coin_et *b) {
    com_sub(&c->c.u, &a->c.u, &b->c.u);
}

int compiler(context_t *ctx, uint8_t range,
             FILE * zk_proof,
             command_set *cmds,
             uint8_t inlen, coin_ct in[],
             uint8_t outlen, coin_ct out[],
             uint8_t extralen, var_ct extra[]) {

    var_ct *variables;
    variables = (var_ct *) malloc(99 * sizeof(var_ct));
    int res;

    res = runner_gen(ctx, zk_proof, cmds, range, variables, extralen, extra,
                     inlen, in, outlen, out);


    free(variables);

    return res;
}


int executor(context_t *ctx, uint8_t range,
             FILE * zk_proof,
             command_set *cmds,
             uint8_t inlen, coin_et in[],
             uint8_t outlen, coin_et out[]) {

    var_et *variables;
    variables = (var_et *) malloc(99 * sizeof(var_et));
    int res;

    res = runner_ver(ctx, zk_proof, cmds, range, variables,
                     inlen, in, outlen, out);

    free(variables);

    return res;
}

void oprint(int64_t len, uint8_t *vec) {
    for (int i = 0; i < len; i++) {
        printf("%d", vec[i]);
    }
    printf("\n");
}

void poly_print(poly *a, int len) {
    cout << "\n";
    for (int i = 0; i < len; i++) {
        cout << a->coef[i] << " ";
    }
    cout << "\n";
}

#endif // ZKSC_LIBRARY