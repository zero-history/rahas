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


#ifndef CIP_SIS_INPUT_H
#define CIP_SIS_INPUT_H

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

void tobinL(poly *b, zint_t v);

void commit(context_t *ctx, poly_n *u, poly k[mk], poly v[mv], poly r[mk]);

int open(context_t *ctx, poly_n *u, poly v[mv], poly k[mk]);

int allocate_rp(rp_t *rp, uint8_t L1, uint8_t d);

void deallocate_rp(rp_t *rp);

void range_gen(context_t *ctx, uint8_t L1, uint8_t d, rp_t *rp, poly_n *u, const zint_t *v, poly k[mk]);

int range_ver(context_t *ctx, poly_n *u, rp_t *rp);

void commit_from_int(context_t *ctx, uint8_t d, poly_n *u, poly k[mk], zint_t v[mv]);

int rcommit(context_t *ctx, uint8_t L1, uint8_t d, rp_t *rp, poly_n *u, zint_t v[mv], poly k[mk]);

#endif //CIP_SIS_INPUT_H
