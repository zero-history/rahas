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

#ifndef CIP_SIS_RANDOM_H
#define CIP_SIS_RANDOM_H

#if(ENABLE_56INT == 0)
#include "params32.h"
#else
#include "params56.h"
#endif
#include "poly.h"

void get_mask(poly *a, uint8_t *seed, int bound_bytes);

void set_mask(poly *a, const uint8_t *seed, int bound_bytes);

void get_alpha_mask(poly *a, uint8_t seed[a_BYTES], zint_t b);

void set_alpha_mask(poly *a, const uint8_t seed[a_BYTES]);

void get_alpha1_mask(poly *a, const zint_t b, const uint8_t L1);

void get_mask_tau(poly *a, uint8_t seed[r_BYTES]);

void get_mask_tau1(poly *a, uint8_t seed[r1_BYTES]);

void get_mask_tau2(poly *a, uint8_t seed[r2_BYTES]);

void get_mask_tau3(poly *a, uint8_t seed[r3_BYTES]);

void set_mask_tau(poly *a, const uint8_t seed[r_BYTES]);

void set_mask_tau1(poly *a, const uint8_t seed[r1_BYTES]);

void set_mask_tau2(poly *a, const uint8_t seed[r2_BYTES]);

void set_mask_tau3(poly *a, const uint8_t seed[r3_BYTES]);

#endif //CIP_SIS_RANDOM_H
