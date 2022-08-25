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

#ifndef CIP_SIS_PACK_H
#define CIP_SIS_PACK_H

#if(ENABLE_56INT == 0)
#include "structs32.h"
#else
#include "structs56.h"
#endif
#include "poly.h"
#include "random.h"
#include "../util.h"
#include "polyvec.h"

#define Q_BYTES       6   // should be able to reduce this even more

void pack_custom_poly_internal(uint8_t *bytes, poly *a, int bound_byte);
void unpack_custom_poly_internal(poly *a, const uint8_t *bytes, int bound_byte);

void pack_poly_ring(uint8_t bytes[n * u_BYTES], poly_n *u);
void unpack_poly_ring(poly_n *u, const uint8_t bytes[n * u_BYTES]);

void pack_poly_ring_custom(uint8_t bytes[], poly_n *u, int p);
void unpack_poly_ring_custom(poly_n *u, const uint8_t bytes[], int p);

void pack_poly_com_custom(uint8_t bytes[], poly_n *u);
void unpack_poly_com_custom(poly_n *u, const uint8_t bytes[]);

void pack_poly_z(uint8_t bytes[z_BYTES],  poly s[L]);
void unpack_poly_z(poly s[L], const uint8_t bytes[z_BYTES]);

void pack_poly_m_R(uint8_t bytes[r2_BYTES],  poly s[mk]);
void unpack_poly_m_R(poly s[mk], const uint8_t bytes[m * r2_BYTES]);

void pack_poly_sig(uint8_t bytes[R_BYTES],  poly s[mk]);
void unpack_poly_sig(poly s[mk], const uint8_t bytes[R_BYTES]);

void pack_custom_poly(uint8_t *bytes,  poly *a);
void unpack_custom_poly(poly *a, const uint8_t *bytes);

#endif //CIP_SIS_PACK_H
