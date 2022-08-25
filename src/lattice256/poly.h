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

#ifndef CIP_SIS_POLY_H
#define CIP_SIS_POLY_H

#if(ENABLE_56INT == 0)
#include "structs32.h"
#else
#include "structs56.h"
#endif

#include "ntt.h"
#include "fips202.h"
#include "shake.h"

#define Q_BYTES       6   // should be able to reduce this even more

zint_t reduceK1(const zint_t a);

zint_t reduceK1_exact(zint_t a);

zint_t highbits(zint_t a, unsigned int p);

zint_t roundup(zint_t a, unsigned int p);

poly poly_from_vec(const zint_t vec[N]);

void poly_set(poly *b, poly *a);

int poly_compare(const poly *s1, const poly *s2);

int poly_highbits_compare(const poly *s1, const poly *s2, unsigned int p);

void poly_set_zero(poly *a, unsigned int start, unsigned int end);

void poly_rot(poly *a, zint_t val, unsigned int i);

int64_t addq(int64_t a);

void poly_highbits(poly *a, poly *b, unsigned int p);

void poly_roundup(poly *a, poly *b, unsigned int p);

void poly_lowbit_clean(poly *a, poly *b, unsigned int p);

void poly_reduce(poly *a);

void poly_reduce_exact(poly *a);

void poly_add(poly *c, poly *a, poly *b);

void poly_sub(poly *c, poly *a, poly *b);

void poly_shift_l(poly *a, unsigned int l);

void poly_ntt(poly *a);

void poly_inv_ntt_to_mont(poly *a);

void poly_inv_ntt(poly *a);

void poly_pointwise_montgomery(poly *c, poly *a, poly *b);

void poly_easy_mul(poly *c, poly *a, unsigned int i, const zint_t value);

int poly_chknorm(const poly *a, zint_t B);
int poly_chknorm_custom(const poly *a, zint_t B, int start, int end);

void poly_challenge(poly *c, const uint8_t seed[SEED_BYTES]);


#endif //CIP_SIS_POLY_H
