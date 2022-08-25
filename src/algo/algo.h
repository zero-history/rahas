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

#ifndef CIP_SIS_ALGO_H
#define CIP_SIS_ALGO_H

#if(ENABLE_56INT == 0)
#include "structs32.h"
#else
#include "structs56.h"
#endif

#include "poly.h"
#include "random.h"
#include "../util.h"
#include "polyvec.h"
#include "pack.h"
#include "input.h"
#include "fips202.h"
#include "shake.h"

int equal_gen_from_int(context_t *ctx, eq_pt *proof,
              com_t *u1, zint_t *v1, mask_t *k1,
              com_t *u2, zint_t *v2, mask_t *k2);

int equal_gen(context_t *ctx, eq_pt *proof,
              com_t *u1, poly v1[mv], mask_t *k1,
              com_t *u2, poly v2[mv], mask_t *k2);

int equal_ver(context_t *ctx, com_t *u1, com_t *u2, eq_pt *proof);

int allocate_neq_proof(neq_pt *proof, uint8_t L1, uint8_t d);

void deallocate_neq_proof(neq_pt *proof);

int nequal_gen(context_t *ctx, uint8_t L1, uint8_t d, neq_pt *proof,
               com_t *u1, zint_t *v1, mask_t *k1,
               com_t *u2, zint_t *v2, mask_t *k2);

int nequal_ver(context_t *ctx, com_t *u1, com_t *u2, neq_pt *proof);

int allocate_leq(leq_proof_t *proof, uint8_t L1, uint8_t d);

void deallocate_leq(leq_proof_t *proof);

int leq_gen(context_t *ctx, uint8_t L1, uint8_t d, leq_proof_t *proof,
            com_t *u1, zint_t *v1, mask_t *k1,
            com_t *u2, zint_t *v2, mask_t *k2);

int leq_ver(context_t *ctx, com_t *u1, com_t *u2, leq_proof_t *proof);

int allocate_geq(geq_proof_t *proof, uint8_t L1, uint8_t d);

void deallocate_geq(geq_proof_t *proof);

int geq_gen(context_t *ctx, uint8_t L1, uint8_t d, geq_proof_t *proof,
            com_t *u1, zint_t *v1, mask_t *k1,
            com_t *u2, zint_t *v2, mask_t *k2);

int geq_ver(context_t *ctx, com_t *u1, com_t *u2, geq_proof_t *proof);

void add_gen(context_t *ctx, uint8_t d,
             com_t *u0, zint_t *v0, mask_t *k0,
             com_t *u1, zint_t *v1, mask_t *k1,
             com_t *u2, zint_t *v2, mask_t *k2);

int add_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2);

void sub_gen(context_t *ctx, uint8_t d,
             com_t *u0, zint_t *v0, mask_t *k0,
             com_t *u1, zint_t *v1, mask_t *k1,
             com_t *u2, zint_t *v2, mask_t *k2);

int sub_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2);

int allocate_mul_proof(mul_pt *proof, uint8_t L1, uint8_t d);

void deallocate_mul_proof(mul_pt *proof);

void mul_gen(context_t *ctx, uint8_t L1, uint8_t d,
             mul_pt *proof,
             com_t *u0, zint_t *v0, mask_t *k0,
             com_t *u1, zint_t *v1, mask_t *k1,
             com_t *u2, zint_t *v2, mask_t *k2);

int mul_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2, mul_pt *proof);

int allocate_div_unsigned_proof(div_upt *proof, uint8_t L1, uint8_t d);

void deallocate_div_unsigned_proof(div_upt *proof);

int div_unsigned_gen(context_t *ctx, uint8_t L1, uint8_t d,
                     div_upt *proof,
                     com_t *u0, zint_t *v0, mask_t *k0,
                     com_t *u3, zint_t *v3, mask_t *k3,
                     com_t *u1, zint_t *v1, mask_t *k1,
                     com_t *u2, zint_t *v2, mask_t *k2);

int div_unsigned_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2, com_t *u3, div_upt *proof);

int allocate_div_signed_proof(div_pt *proof, uint8_t L1, uint8_t d);

void deallocate_div_signed_proof(div_pt *proof);

int div_signed_gen(context_t *ctx, uint8_t L1, uint8_t d,
                   div_pt *proof,
                   com_t *u0, zint_t *v0, mask_t *k0,
                   com_t *u3, zint_t *v3, mask_t *k3,
                   com_t *u1, zint_t *v1, mask_t *k1,
                   com_t *u2, zint_t *v2, mask_t *k2);

int div_signed_ver(context_t *ctx, com_t *u0, com_t *u1, com_t *u2, com_t *u3, div_pt *proof);

void com_to_bytes(uint8_t bytes[COM_BYTES], com_t *c);

void bytes_to_com(com_t *c, uint8_t bytes[COM_BYTES]);

int get_rp_size(rp_t *rp);

void rp_to_bytes(uint8_t bytes[], rp_t *rp);

void bytes_to_rp(rp_t *rp, uint8_t bytes[]);

void copy_rp(rp_t *out, rp_t *in);

int get_mul_pt_size(mul_pt *proof);

void mul_pt_to_bytes(uint8_t bytes[], mul_pt *proof);

void bytes_to_mul_pt(mul_pt *proof, uint8_t bytes[]);

void eq_pt_to_bytes(uint8_t bytes[EQ_PT_BYTES], eq_pt *proof);

void bytes_to_eq_pt(eq_pt *proof, uint8_t bytes[EQ_PT_BYTES]);

int get_neq_pt_size(neq_pt *proof);

void neq_pt_to_bytes(uint8_t bytes[], neq_pt *proof);

void bytes_to_neq_pt(neq_pt *proof, uint8_t bytes[]);

int get_div_upt_size(div_upt *proof);

void div_upt_to_bytes(uint8_t bytes[], div_upt *proof);

void bytes_to_div_upt(div_upt *proof, uint8_t bytes[]);

int get_div_pt_size(div_pt *proof);

void div_pt_to_bytes(uint8_t bytes[], div_pt *proof);

void bytes_to_div_pt(div_pt *proof, uint8_t bytes[]);

#endif //CIP_SIS_ALGO_H
