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
 * Get bytes of a commitment
 * @param bytes - output (COM_BYTES)
 * @param c - inputs
 */
void com_to_bytes(uint8_t bytes[COM_BYTES], com_t *c) {
    pack_poly_com_custom(bytes, c);
}


/**
 * Get a commitment from bytes
 * @param c - output
 * @param bytes - input (COM_BYTES)
 */
void bytes_to_com(com_t *c, uint8_t bytes[COM_BYTES]) {
    unpack_poly_com_custom(c, bytes);
}


/**
 * Get byte size of a range proof
 * @param rp - input
 * @return size
 */
int get_rp_size(rp_t *rp) {
    return (2 + (rp->d * rp->L1 * a_BYTES) + R_BYTES + t1_HIGHBITS + SEED_BYTES + HINTBYTES);
}


/**
 * Get bytes of a range proof
 * @param bytes - output (use get_rp_size)
 * @param rp - input
 */
void rp_to_bytes(uint8_t bytes[], rp_t *rp) {
    int i, j;
    bytes[0] = rp->L1;
    bytes[1] = rp->d;
    int pointer = 2;
    for (i = 0; i < rp->d; i++)
        for (j = 0; j < rp->L1; j++) {
            pack_custom_poly_internal(bytes + pointer, &rp->z[i][j], ALPHA_BITS + 1);
            pointer += a_BYTES;
        }
    pack_poly_m_R(bytes + pointer, rp->s);
    pointer += R_BYTES;
    pack_poly_ring_custom(bytes + pointer, &rp->t1, K1 - t1_ERROR);
    pointer += t1_HIGHBITS;
    pack_poly_ring_custom(bytes + pointer, &rp->t2_hints, HINTBITS);
    pointer += HINTBYTES;
    memcpy(bytes + pointer, rp->x2, SEED_BYTES);
}


/**
 * Copy a range proof "in" to "out"
 * @param out
 * @param in
 */
void copy_rp(rp_t *out, rp_t *in) {
    int i, j;
    out->L1 = in->L1;
    out->d = in->d;
    for (i = 0; i < in->d; i++)
        for (j = 0; j < in->L1; j++) {
            poly_set(&out->z[i][j], &in->z[i][j]);
        }
    for (i = 0; i < mk; i++)
        poly_set(&out->s[i], &in->s[i]);
    poly_n_set(&out->t1, &in->t1);
    poly_n_set(&out->t2_hints, &in->t2_hints);
    memcpy(out->x2, in->x2, SEED_BYTES);
}


/**
 * Get a range proof from bytes
 * @param rp - output
 * @param bytes - input
 */
void bytes_to_rp(rp_t *rp, uint8_t bytes[]) {
    int i, j;
    rp->L1 = bytes[0];
    rp->d = bytes[1];
    int pointer = 2;
    for (i = 0; i < rp->d; i++)
        for (j = 0; j < rp->L1; j++) {
            unpack_custom_poly_internal(&rp->z[i][j], bytes + pointer, ALPHA_BITS + 1);
            pointer += a_BYTES;
        }
    unpack_poly_m_R(rp->s, bytes + pointer);
    pointer += R_BYTES;
    unpack_poly_ring_custom(&rp->t1, bytes + pointer, K1 - t1_ERROR);
    pointer += t1_HIGHBITS;
    unpack_poly_ring_custom(&rp->t2_hints, bytes + pointer, HINTBITS);
    pointer += HINTBYTES;
    memcpy(rp->x2, bytes + pointer, SEED_BYTES);
}


/**
 * Get byte size of a multiplication proof
 * @param proof - input
 * @return size
 */
int get_mul_pt_size(mul_pt *proof) {
    int a1_BYTES = ((N)/8 * (proof->L1 + EXTRA_BITS + 1));
    return 2 + (2 * proof->d * a1_BYTES) + R_BYTES + t1_HIGHBITS + SEED_BYTES + HINTBYTES;
}


/**
 * Get bytes of a multiplication proof
 * @param bytes - output (use get_mul_pt_size)
 * @param proof - input
 */
void mul_pt_to_bytes(uint8_t bytes[], mul_pt *proof) {
    int i;
    int a1_BYTES = ((N)/8 * (proof->L1 + EXTRA_BITS + 1));
    bytes[0] = proof->L1;
    bytes[1] = proof->d;
    int pointer = 2;
    for (i = 0; i < proof->d; i++) {
        pack_custom_poly_internal(bytes + pointer, &proof->z1[i], (proof->L1 + EXTRA_BITS) + 1);
        pointer += a1_BYTES;
        pack_custom_poly_internal(bytes + pointer, &proof->z2[i], (proof->L1 + EXTRA_BITS) + 1);
        pointer += a1_BYTES;
    }
    pack_poly_m_R(bytes + pointer, proof->s);
    pointer += R_BYTES;
    pack_poly_ring_custom(bytes + pointer, &proof->t1, K1 - t1_ERROR);
    pointer += t1_HIGHBITS;
    pack_poly_ring_custom(bytes + pointer, &proof->t2_hints, HINTBITS);
    pointer += HINTBYTES;
    memcpy(bytes + pointer, proof->x2, SEED_BYTES);
}


/**
 * Get a multiplication proofs from bytes
 * @param proof - output
 * @param bytes - input
 */
void bytes_to_mul_pt(mul_pt *proof, uint8_t bytes[]) {
    int i;
    proof->L1 = bytes[0];
    proof->d = bytes[1];
    int a1_BYTES = ((N)/8 * (proof->L1 + EXTRA_BITS + 1));
    int ALPHA1_BITS = proof->L1 + EXTRA_BITS;
    int pointer = 2;
    for (i = 0; i < proof->d; i++) {
        unpack_custom_poly_internal(&proof->z1[i], bytes + pointer, ALPHA1_BITS + 1);
        pointer += a1_BYTES;
        unpack_custom_poly_internal(&proof->z2[i], bytes + pointer, ALPHA1_BITS + 1);
        pointer += a1_BYTES;
    }
    unpack_poly_m_R(proof->s, bytes + pointer);
    pointer += R_BYTES;
    unpack_poly_ring_custom(&proof->t1, bytes + pointer, K1 - t1_ERROR);
    pointer += t1_HIGHBITS;
    unpack_poly_ring_custom(&proof->t2_hints, bytes + pointer, HINTBITS);
    pointer += HINTBYTES;
    memcpy(proof->x2, bytes + pointer,  SEED_BYTES);
}


/**
 * Get bytes of a "equal" statement
 * @param bytes - output (EQ_PT_BYTES)
 * @param proof - input
 */
void eq_pt_to_bytes(uint8_t bytes[EQ_PT_BYTES], eq_pt *proof) {
    pack_poly_m_R(bytes, proof->s);
    memcpy(bytes + R_BYTES, proof->x, SEED_BYTES);
    pack_poly_ring_custom(bytes + R_BYTES + SEED_BYTES, &proof->t_hints, HINTBITS);
}


/**
 * Get a "equal" statement from bytes
 * @param proof - output
 * @param bytes - input
 */
void bytes_to_eq_pt(eq_pt *proof, uint8_t bytes[EQ_PT_BYTES]) {
    unpack_poly_m_R(proof->s, bytes);
    memcpy(proof->x, bytes + R_BYTES, SEED_BYTES);
    unpack_poly_ring_custom(&proof->t_hints, bytes + R_BYTES + SEED_BYTES, HINTBITS);
}


/**
 * Get byte size of a "not equal" statement
 * @param proof - input
 * @return size
 */
int get_neq_pt_size(neq_pt *proof) {
    return (COM_BYTES + get_mul_pt_size(&proof->proof) + get_rp_size(&proof->rp));
}


/**
 * Get bytes of a "not equal" statement
 * @param bytes - output (use get_neq_pt_size)
 * @param proof - input
 */
void neq_pt_to_bytes(uint8_t bytes[], neq_pt *proof) {
    int pointer;
    pack_poly_com_custom(bytes, &proof->u);
    pointer = COM_BYTES;
    mul_pt_to_bytes(bytes + pointer, &proof->proof);
    pointer += get_mul_pt_size(&proof->proof);
    rp_to_bytes(bytes + pointer, &proof->rp);
}


/**
 * Get a "not equal" statement from bytes
 * @param proof - output
 * @param bytes - input
 */
void bytes_to_neq_pt(neq_pt *proof, uint8_t bytes[]) {
    int pointer;
    unpack_poly_com_custom(&proof->u, bytes);
    pointer = COM_BYTES;
    bytes_to_mul_pt(&proof->proof, bytes + pointer);
    pointer += get_mul_pt_size(&proof->proof);
    bytes_to_rp( &proof->rp, bytes + pointer);
}


/**
 * Get byte size of a signed division proof
 * @param proof - input
 * @return size
 */
int get_div_upt_size(div_upt *proof) {
    int a1_BYTES = (N/8 * (proof->L1 + EXTRA_BITS + 1));
    return (2 + (2 * proof->rp.d * a1_BYTES) + R_BYTES + t1_HIGHBITS + HINTBYTES + SEED_BYTES + get_rp_size(&proof->rp));
}


/**
 * Get bytes of a unsigned division proof
 * @param bytes - output (use get_div_upt_size)
 * @param proof - input
 */
void div_upt_to_bytes(uint8_t bytes[], div_upt *proof) {
    int i;
    bytes[0] = proof->L1;
    bytes[1] = proof->d;
    int a1_BYTES = ((N)/8 * (proof->L1 + EXTRA_BITS + 1));
    int ALPHA1_BITS = (proof->L1 + EXTRA_BITS);
    int pointer = 2;
    for (i = 0; i < proof->d; i++) {
        pack_custom_poly_internal(bytes + pointer, &proof->z0[i], ALPHA1_BITS + 1);
        pointer += a1_BYTES;
        pack_custom_poly_internal(bytes + pointer, &proof->z2[i], ALPHA1_BITS + 1);
        pointer += a1_BYTES;
    }
    pack_poly_m_R(bytes + pointer, proof->s);
    pointer += R_BYTES;
    pack_poly_ring_custom(bytes + pointer, &proof->t1, K1 - t1_ERROR);
    pointer += t1_HIGHBITS;
    pack_poly_ring_custom(bytes + pointer, &proof->t2_hints, HINTBITS);
    pointer += HINTBYTES;
    memcpy(bytes + pointer, proof->x2, SEED_BYTES);
    pointer += SEED_BYTES;
    rp_to_bytes(bytes + pointer, &proof->rp);
}


/**
 * Get a unsigned division proof from bytes
 * @param proof - output
 * @param bytes - input
 */
void bytes_to_div_upt(div_upt *proof, uint8_t bytes[]) {
    int i;
    proof->L1 = bytes[0];
    proof->d = bytes[1];
    int a1_BYTES = ((N)/8 * (proof->L1 + EXTRA_BITS + 1));
    int ALPHA1_BITS = proof->L1 + EXTRA_BITS;
    int pointer = 2;
    for (i = 0; i < proof->d; i++) {
        unpack_custom_poly_internal(&proof->z0[i], bytes + pointer, ALPHA1_BITS + 1);
        pointer += a1_BYTES;
        unpack_custom_poly_internal(&proof->z2[i], bytes + pointer, ALPHA1_BITS + 1);
        pointer += a1_BYTES;
    }
    unpack_poly_m_R(proof->s, bytes + pointer);
    pointer += R_BYTES;
    unpack_poly_ring_custom(&proof->t1, bytes + pointer, K1 - t1_ERROR);
    pointer += t1_HIGHBITS;
    unpack_poly_ring_custom(&proof->t2_hints, bytes + pointer, HINTBITS);
    pointer += HINTBYTES;
    memcpy(proof->x2, bytes + pointer, SEED_BYTES);
    pointer += SEED_BYTES;
    bytes_to_rp(&proof->rp, bytes + pointer);
}


/**
 * Get byte size of a signed division proof
 * @param proof - input
 * @return size
 */
int get_div_pt_size(div_pt *proof) {
    int a1_BYTES = ((N)/8 * (proof->L1 + EXTRA_BITS + 1));
    return (2 + (2 * proof->rp.d * a1_BYTES) + R_BYTES + (2 * COM_BYTES) + t1_HIGHBITS + HINTBYTES + SEED_BYTES +
            get_rp_size(&proof->rp) + get_rp_size(&proof->rp_v3) + (2 * get_mul_pt_size(&proof->u2_proof)));
}


/**
 * Get bytes of signed division
 * @param bytes - output (use get_div_pt_size)
 * @param proof - input
 */
void div_pt_to_bytes(uint8_t bytes[], div_pt *proof) {
    int i;
    bytes[0] = proof->L1;
    bytes[1] = proof->d;
    int a1_BYTES = ((N)/8 * (proof->L1 + EXTRA_BITS + 1));
    int ALPHA1_BITS = (proof->L1 + EXTRA_BITS);
    int pointer = 2;
    for (i = 0; i < proof->d; i++) {
        pack_custom_poly_internal(bytes + pointer, &proof->z0[i], ALPHA1_BITS + 1);
        pointer += a1_BYTES;
        pack_custom_poly_internal(bytes + pointer, &proof->z2[i], ALPHA1_BITS + 1);
        pointer += a1_BYTES;
    }
    pack_poly_m_R(bytes + pointer, proof->s);
    pointer += R_BYTES;
    pack_poly_ring_custom(bytes + pointer, &proof->t1, K1 - t1_ERROR);
    pointer += t1_HIGHBITS;
    pack_poly_ring_custom(bytes + pointer, &proof->t2_hints, HINTBITS);
    pointer += HINTBYTES;
    pack_poly_com_custom(bytes + pointer, &proof->u22);
    pointer += COM_BYTES;
    pack_poly_com_custom(bytes + pointer, &proof->u32);
    pointer += COM_BYTES;
    memcpy(bytes + pointer, proof->x2, SEED_BYTES);
    pointer += SEED_BYTES;
    rp_to_bytes(bytes + pointer, &proof->rp);
    pointer += get_rp_size(&proof->rp);
    rp_to_bytes(bytes + pointer, &proof->rp_v3);
    pointer += get_rp_size(&proof->rp_v3);
    mul_pt_to_bytes(bytes + pointer, &proof->u2_proof);
    pointer += get_mul_pt_size(&proof->u2_proof);
    mul_pt_to_bytes(bytes + pointer, &proof->u3_proof);
}


/**
 * Get a signed division proof from bytes
 * @param proof - output
 * @param bytes - input
 */
void bytes_to_div_pt(div_pt *proof, uint8_t bytes[]) {
    int i;
    proof->L1 = bytes[0];
    proof->d = bytes[1];
    int a1_BYTES = ((N)/8 * (proof->L1 + EXTRA_BITS + 1));
    int ALPHA1_BITS = proof->L1 + EXTRA_BITS;
    int pointer = 2;
    for (i = 0; i < proof->d; i++) {
        unpack_custom_poly_internal(&proof->z0[i], bytes + pointer, ALPHA1_BITS + 1);
        pointer += a1_BYTES;
        unpack_custom_poly_internal(&proof->z2[i], bytes + pointer, ALPHA1_BITS + 1);
        pointer += a1_BYTES;
    }
    unpack_poly_m_R(proof->s, bytes + pointer);
    pointer += R_BYTES;
    unpack_poly_ring_custom(&proof->t1, bytes + pointer, K1 - t1_ERROR);
    pointer += t1_HIGHBITS;
    unpack_poly_ring_custom(&proof->t2_hints, bytes + pointer, HINTBITS);
    pointer += HINTBYTES;
    unpack_poly_com_custom(&proof->u22, bytes + pointer);
    pointer += COM_BYTES;
    unpack_poly_com_custom(&proof->u32, bytes + pointer);
    pointer += COM_BYTES;
    memcpy(proof->x2, bytes + pointer,  SEED_BYTES);
    pointer += SEED_BYTES;
    bytes_to_rp(&proof->rp, bytes + pointer);
    pointer += get_rp_size(&proof->rp);
    bytes_to_rp(&proof->rp_v3, bytes + pointer);
    pointer += get_rp_size(&proof->rp_v3);
    bytes_to_mul_pt(&proof->u2_proof, bytes + pointer);
    pointer += get_mul_pt_size(&proof->u2_proof);
    bytes_to_mul_pt(&proof->u3_proof, bytes + pointer);
}

