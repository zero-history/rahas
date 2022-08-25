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

#ifndef ZKSC_STRUCTS32_H
#define ZKSC_STRUCTS32_H

#include "params32.h"

typedef struct poly_struct {
    zint_t coef[N];
} poly;

typedef struct poly_n_struct {
    poly vec[n];
} poly_n;

typedef struct poly_m_struct {
    poly vec[m];
} poly_m;

typedef struct context_struct {
    poly_m H[n];
    uint8_t seed[SEED_BYTES];
    poly one_N;
} context_t;

typedef poly_n com_t;

typedef struct mask_struct {
    poly k[mk];
} mask_t;

typedef struct rp_struct {
    uint8_t L1;
    uint8_t d = 0;
    poly **z = nullptr;
    poly s[mk];
    poly_n t1;
    poly_n t2_hints;
    uint8_t x2[SEED_BYTES];
} rp_t;

typedef struct mul_proof_struct {
    uint8_t L1;
    uint8_t d = 0;
    poly *z1 = nullptr;
    poly *z2 = nullptr;
    poly s[mk];
    poly_n t1;
    poly_n t2_hints;
    uint8_t x2[SEED_BYTES];
} mul_pt;

typedef struct eq_proof_struct {
    poly s[mk];
    uint8_t x[SEED_BYTES];
    poly_n t_hints;
} eq_pt;

typedef struct neq_proof_struct {
    poly_n u;
    mul_pt proof;
    rp_t rp;
} neq_pt;

typedef rp_t leq_proof_t;

typedef rp_t geq_proof_t;

typedef struct div_proof_struct {
    uint8_t L1;
    uint8_t d = 0;
    poly *z0 = nullptr;
    poly *z2 = nullptr;
    poly s[mk];
    poly_n t1;
    poly_n t2_hints;
    uint8_t x2[SEED_BYTES];
    leq_proof_t rp;
} div_upt;

typedef struct div_signed_proof_struct {
    uint8_t L1;
    uint8_t d = 0;
    poly *z0 = nullptr;
    poly *z2 = nullptr;
    poly s[mk];
    poly_n t1;
    poly_n t2_hints;
    poly_n u22;
    poly_n u32;
    uint8_t x2[SEED_BYTES];
    leq_proof_t rp;
    mul_pt u2_proof;
    mul_pt u3_proof;
    rp_t rp_v3;
} div_pt;


#define RANGE           32  // 2^32
#define ZKSC_OK         0
#define ZKSC_ER         (-1)
#define MASKKEY_BYTES    (mk * r_BYTES)
#define COM_BYTES       u_HIGHBITS
#define EQ_PT_BYTES     (R_BYTES + SEED_BYTES + HINTBYTES)
#define PKID            28 // ORIGAMI_EID_BYTES

typedef struct in_compiling_struct {
    com_t u;
    zint_t v = 0;
    mask_t k;
    uint8_t is_set_pk = 0;
    uint8_t pk[PKID];
} in_ct;

typedef struct in_execution_struct {
    com_t u; // for confidential integers
    zint_t v = 0; // for integers
    uint8_t is_set_pk = 0; // for public keys
    uint8_t pk[PKID];
} in_et;

typedef struct coin_compiling_struct {
    in_ct c;
    rp_t rp;
} coin_ct;

typedef struct coin_execution_struct {
    in_et c;
    rp_t rp;
} coin_et;

typedef struct variable_compile_struct {
    uint8_t type = 0;
    in_ct data;
} var_ct;

typedef struct variable_execution_struct {
    uint8_t type = 0;
    in_et data;
} var_et;


#endif //ZKSC_STRUCTS32_H
