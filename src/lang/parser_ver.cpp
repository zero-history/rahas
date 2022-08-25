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

#include "parser.h"
#include "executing_functions.h"


// 0 - program begin
// 1 - if/else
int runner_ver(context_t *ctx, FILE *zk_proof, command_set *cmds,
               uint8_t L1, var_et *variables,
               uint8_t inlen, coin_et in[],
               uint8_t outlen, coin_et out[]) {
    char *line = nullptr;
    int res;
    int branch = 0;
    int loop = 0;

    int stack_pointer = 0;

    while (stack_pointer != -1) {
        DEBUG_PRINT(("\nVer: Retrieved : %d %d %d\n", stack_pointer, cmds->set[stack_pointer].type, cmds->set[stack_pointer].subtype));

        switch (cmds->set[stack_pointer].type) {
            // Returning Commands
            case 0:
                if (cmds->set[stack_pointer].subtype == return_ok_byte[1]) {
                    DEBUG_PRINT(("Exiting with ok %d\n", branch));
                    FREE_LINE(line);
                    return 1;
                }
                else if (cmds->set[stack_pointer].subtype == return_error_byte[1]) {
                    DEBUG_PRINT(("Exiting with error %d\n", branch));
                    FREE_LINE(line);
                    return -1;
                }
                else {
                    DEBUG_PRINT(("Syntax error 0: open branches %d\n", branch));
                    FREE_LINE(line);
                    return CONTRACT_SYNTAX_ERR;
                }
                break;

                // Set
            case 1:
                if (cmds->set[stack_pointer].subtype == set_int_byte[1]) {
                    res = set_int_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == set_cint_byte[1]) {
                    res = set_cint_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == set_cint_from_in_byte[1]) {
                    set_cint_from_in_ver(&cmds->set[stack_pointer], variables, in);
                }
                else if (cmds->set[stack_pointer].subtype == set_cint_from_out_byte[1]) {
                    set_cint_from_out_ver(&cmds->set[stack_pointer], variables, out);
                }
                else if (cmds->set[stack_pointer].subtype ==  set_cint_from_in_pk_byte[1]) {
                    res = set_cint_from_in_pk_ver(&cmds->set[stack_pointer], variables, inlen, in);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype ==  set_cint_from_out_pk_byte[1]) {
                    res = set_cint_from_out_pk_ver(&cmds->set[stack_pointer], variables, outlen, out);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype ==  set_pk_byte[1]) {
                    res = set_pk_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype ==  set_pk_from_in_byte[1]) {
                    set_pk_from_in_ver(&cmds->set[stack_pointer], variables, in);
                }
                else if (cmds->set[stack_pointer].subtype ==  set_pk_from_out_byte[1]) {
                    set_pk_from_out_ver(&cmds->set[stack_pointer], variables, out);
                }
                else if (cmds->set[stack_pointer].subtype ==  set_int_from_inlen_byte[1]) {
                    set_int_from_lenths_ver(&cmds->set[stack_pointer], variables, inlen);
                }
                else if (cmds->set[stack_pointer].subtype ==  set_int_from_outlen_byte[1]) {
                    set_int_from_lenths_ver(&cmds->set[stack_pointer], variables, outlen);
                }
                else if (cmds->set[stack_pointer].subtype ==  set_cint_from_int_byte[1]) {
                    res = set_cint_from_int_ver(ctx, &cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else {
                    DEBUG_PRINT(("Syntax error 1: open branches %d\n", branch));
                    FREE_LINE(line);
                    return CONTRACT_SYNTAX_ERR;
                }
                stack_pointer = cmds->set[stack_pointer].next_success;
                break;

                // Copy
            case 2:
                if (cmds->set[stack_pointer].subtype == copy_int_byte[1]) {
                    res = copy_int_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype ==  copy_cint_byte[1]) {
                    res = copy_cint_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else {
                    DEBUG_PRINT(("Syntax error 2: open branches %d\n", branch));
                    FREE_LINE(line);
                    return CONTRACT_SYNTAX_ERR;
                }
                stack_pointer = cmds->set[stack_pointer].next_success;
                break;

                // Arithmetic
            case 3:
                if (cmds->set[stack_pointer].subtype == add_int_byte[1]) {
                    res = add_int_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == add_cint_byte[1]) {
                    res = add_cint_ver(ctx, L1, &cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == sub_int_byte[1]) {
                    res = sub_int_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == sub_cint_byte[1]) {
                    res = sub_cint_ver(ctx, L1, &cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == mul_int_byte[1]) {
                    res = mul_int_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == mul_cint_byte[1]) {
                    res = mul_cint_ver(ctx, L1, &cmds->set[stack_pointer], variables, zk_proof);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == div_int_byte[1]) {
                    res = div_int_ver(&cmds->set[stack_pointer], variables);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == divu_cint_byte[1]) {
                    res = divu_cint_ver(ctx, L1, &cmds->set[stack_pointer], variables, zk_proof);
                    if (res != 0) return res;
                }
                else if (cmds->set[stack_pointer].subtype == div_cint_byte[1]) {
                    res = div_cint_ver(ctx, L1, &cmds->set[stack_pointer], variables, zk_proof);
                    if (res != 0) return res;
                }
                else {
                    DEBUG_PRINT(("Syntax error: arithmetic %d\n", branch));
                    FREE_LINE(line);
                    return CONTRACT_SYNTAX_ERR;
                }
                stack_pointer = cmds->set[stack_pointer].next_success;
                break;

                // If
            case 4:
                if (cmds->set[stack_pointer].subtype == if_end_byte[1]) {
                    branch--;
                    stack_pointer = cmds->set[stack_pointer].next_success;
                }
                    // This will only happen if ``if'' is successful.
                else if (cmds->set[stack_pointer].subtype == else_byte[1]) {
                    stack_pointer = cmds->set[stack_pointer].next_failure;
                }
                else if (cmds->set[stack_pointer].subtype == if_eq_int_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_INT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_INT) {
                        DEBUG_PRINT(("int type error in if_eq_int: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_INT_TYPE_ERR;
                    }
                    branch++;
                    if (variables[cmds->set[stack_pointer].var1].data.v == variables[cmds->set[stack_pointer].var2].data.v) {
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == if_neq_int_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_INT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_INT) {
                        DEBUG_PRINT(("int type error in if_neq_int: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_INT_TYPE_ERR;
                    }
                    branch++;
                    if (variables[cmds->set[stack_pointer].var1].data.v != variables[cmds->set[stack_pointer].var2].data.v) {
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == if_leq_int_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_INT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_INT) {
                        DEBUG_PRINT(("int type error in if_leq_int: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_INT_TYPE_ERR;
                    }
                    branch++;
                    if (variables[cmds->set[stack_pointer].var1].data.v <= variables[cmds->set[stack_pointer].var2].data.v) {
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == if_geq_int_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_INT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_INT) {
                        DEBUG_PRINT(("int type error in if_geq_int: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_INT_TYPE_ERR;
                    }
                    branch++;
                    if (variables[cmds->set[stack_pointer].var1].data.v >= variables[cmds->set[stack_pointer].var2].data.v) {
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == if_eq_pk_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_PK ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_PK) {
                        DEBUG_PRINT(("int type error in if_eq_pk: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_PK_TYPE_ERR;
                    }
                    branch++;
                    if (memcmp(variables[cmds->set[stack_pointer].var1].data.pk, \
                               variables[cmds->set[stack_pointer].var2].data.pk, PKID) == 0){
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == if_neq_pk_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_PK ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_PK) {
                        DEBUG_PRINT(("int type error in if_neq_pk: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_PK_TYPE_ERR;
                    }
                    branch++;
                    if (memcmp(variables[cmds->set[stack_pointer].var1].data.pk, \
                               variables[cmds->set[stack_pointer].var2].data.pk, PKID) != 0){
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == if_eq_cint_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_CINT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_CINT) {
                        DEBUG_PRINT(("int type error in if_eq_cint: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_CINT_TYPE_ERR;
                    }
                    branch++;
                    if (fgetc(zk_proof) == 1) {
                        eq_pt proof;
                        uint8_t bytes[EQ_PT_BYTES];
                        for (int l = 0; l < EQ_PT_BYTES; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_eq_pt(&proof, bytes);
                        res = equal_ver(ctx,
                                            &variables[cmds->set[stack_pointer].var1].data.u,
                                            &variables[cmds->set[stack_pointer].var2].data.u,
                                            &proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        neq_pt proof;
                        allocate_neq_proof(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_neq_pt(&proof, bytes);
                        res = nequal_ver(ctx,
                                             &variables[cmds->set[stack_pointer].var1].data.u,
                                             &variables[cmds->set[stack_pointer].var2].data.u,
                                             &proof);
                        deallocate_neq_proof(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }

                }
                else if (cmds->set[stack_pointer].subtype == if_neq_cint_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_CINT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_CINT) {
                        DEBUG_PRINT(("int type error in if_neq_cint: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_CINT_TYPE_ERR;
                    }
                    branch++;
                    if (fgetc(zk_proof) == 1) {
                        neq_pt proof;
                        allocate_neq_proof(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_neq_pt(&proof, bytes);
                        res = nequal_ver(ctx,
                                             &variables[cmds->set[stack_pointer].var1].data.u,
                                             &variables[cmds->set[stack_pointer].var2].data.u,
                                             &proof);
                        deallocate_neq_proof(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        eq_pt proof;
                        uint8_t bytes[EQ_PT_BYTES];
                        for (int l = 0; l < EQ_PT_BYTES; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_eq_pt(&proof, bytes);
                        res = equal_ver(ctx,
                                            &variables[cmds->set[stack_pointer].var1].data.u,
                                            &variables[cmds->set[stack_pointer].var2].data.u,
                                            &proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == if_leq_cint_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_CINT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_CINT) {
                        DEBUG_PRINT(("int type error in if_leq_cint: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_CINT_TYPE_ERR;
                    }
                    branch++;
                    if (fgetc(zk_proof) == 1) {
                        leq_proof_t proof;
                        allocate_leq(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_rp(&proof, bytes);
                        res = leq_ver(ctx,
                                          &variables[cmds->set[stack_pointer].var1].data.u,
                                          &variables[cmds->set[stack_pointer].var2].data.u,
                                          &proof);
                        deallocate_leq(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        geq_proof_t proof;
                        allocate_geq(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_rp(&proof, bytes);
                        res = geq_ver(ctx,
                                          &variables[cmds->set[stack_pointer].var1].data.u,
                                          &variables[cmds->set[stack_pointer].var2].data.u,
                                          &proof);
                        deallocate_geq(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == if_geq_cint_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_CINT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_CINT) {
                        DEBUG_PRINT(("int type error in if_geq_cint: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_CINT_TYPE_ERR;
                    }
                    branch++;
                    if (fgetc(zk_proof) == 1) {
                        geq_proof_t proof;
                        allocate_geq(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_rp(&proof, bytes);
                        res = geq_ver(ctx,
                                          &variables[cmds->set[stack_pointer].var1].data.u,
                                          &variables[cmds->set[stack_pointer].var2].data.u,
                                          &proof);
                        deallocate_geq(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        leq_proof_t proof;
                        allocate_leq(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_rp(&proof, bytes);
                        res = leq_ver(ctx,
                                          &variables[cmds->set[stack_pointer].var1].data.u,
                                          &variables[cmds->set[stack_pointer].var2].data.u,
                                          &proof);
                        deallocate_leq(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                break;

                // WHILE
            case 5:
                if (cmds->set[stack_pointer].subtype == while_end_byte[1]) {
                    loop--;
                    stack_pointer = cmds->set[stack_pointer].next_success;
                }
                else if (cmds->set[stack_pointer].subtype == while_int_eq_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_INT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_INT) {
                        DEBUG_PRINT(("int type error in while_eq_int: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_INT_TYPE_ERR;
                    }
                    loop++;
                    if (variables[cmds->set[stack_pointer].var1].data.v == variables[cmds->set[stack_pointer].var2].data.v) {
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == while_int_neq_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_INT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_INT) {
                        DEBUG_PRINT(("int type error in while_neq_int: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_INT_TYPE_ERR;
                    }
                    loop++;
                    if (variables[cmds->set[stack_pointer].var1].data.v != variables[cmds->set[stack_pointer].var2].data.v) {
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == while_int_leq_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_INT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_INT) {
                        DEBUG_PRINT(("int type error in while_leq_int: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_INT_TYPE_ERR;
                    }
                    loop++;
                    if (variables[cmds->set[stack_pointer].var1].data.v <= variables[cmds->set[stack_pointer].var2].data.v) {
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == while_int_geq_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_INT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_INT) {
                        DEBUG_PRINT(("int type error in while_geq_int: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_INT_TYPE_ERR;
                    }
                    loop++;
                    if (variables[cmds->set[stack_pointer].var1].data.v >= variables[cmds->set[stack_pointer].var2].data.v) {
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == while_cint_eq_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_CINT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_CINT) {
                        DEBUG_PRINT(("int type error in while_eq_cint: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_CINT_TYPE_ERR;
                    }
                    loop++;
                    if (fgetc(zk_proof) == 1) {
                        eq_pt proof;
                        uint8_t bytes[EQ_PT_BYTES];
                        for (int l = 0; l < EQ_PT_BYTES; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_eq_pt(&proof, bytes);
                        res = equal_ver(ctx,
                                        &variables[cmds->set[stack_pointer].var1].data.u,
                                        &variables[cmds->set[stack_pointer].var2].data.u,
                                        &proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        neq_pt proof;
                        allocate_neq_proof(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_neq_pt(&proof, bytes);
                        res = nequal_ver(ctx,
                                         &variables[cmds->set[stack_pointer].var1].data.u,
                                         &variables[cmds->set[stack_pointer].var2].data.u,
                                         &proof);
                        deallocate_neq_proof(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }

                }
                else if (cmds->set[stack_pointer].subtype == while_cint_neq_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_CINT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_CINT) {
                        DEBUG_PRINT(("int type error in while_neq_cint: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_CINT_TYPE_ERR;
                    }
                    loop++;
                    if (fgetc(zk_proof) == 1) {
                        neq_pt proof;
                        allocate_neq_proof(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_neq_pt(&proof, bytes);
                        res = nequal_ver(ctx,
                                         &variables[cmds->set[stack_pointer].var1].data.u,
                                         &variables[cmds->set[stack_pointer].var2].data.u,
                                         &proof);
                        deallocate_neq_proof(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        eq_pt proof;
                        uint8_t bytes[EQ_PT_BYTES];
                        for (int l = 0; l < EQ_PT_BYTES; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_eq_pt(&proof, bytes);
                        res = equal_ver(ctx,
                                        &variables[cmds->set[stack_pointer].var1].data.u,
                                        &variables[cmds->set[stack_pointer].var2].data.u,
                                        &proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == while_cint_leq_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_CINT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_CINT) {
                        DEBUG_PRINT(("int type error in while_leq_cint: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_CINT_TYPE_ERR;
                    }
                    loop++;
                    if (fgetc(zk_proof) == 1) {
                        leq_proof_t proof;
                        allocate_leq(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_rp(&proof, bytes);
                        res = leq_ver(ctx,
                                      &variables[cmds->set[stack_pointer].var1].data.u,
                                      &variables[cmds->set[stack_pointer].var2].data.u,
                                      &proof);
                        deallocate_leq(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        geq_proof_t proof;
                        allocate_geq(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_rp(&proof, bytes);
                        res = geq_ver(ctx,
                                      &variables[cmds->set[stack_pointer].var1].data.u,
                                      &variables[cmds->set[stack_pointer].var2].data.u,
                                      &proof);
                        deallocate_geq(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                else if (cmds->set[stack_pointer].subtype == while_cint_geq_byte[1]) {
                    if (variables[cmds->set[stack_pointer].var1].type != TYPE_CINT ||
                        variables[cmds->set[stack_pointer].var2].type != TYPE_CINT) {
                        DEBUG_PRINT(("int type error in while_leq_cint: %d %d\n", variables[cmds->set[stack_pointer].var2].type, \
                        variables[cmds->set[stack_pointer].var1].type));
                        return CONTRACT_CINT_TYPE_ERR;
                    }
                    loop++;
                    if (fgetc(zk_proof) == 1) {
                        geq_proof_t proof;
                        allocate_geq(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_rp(&proof, bytes);
                        res = geq_ver(ctx,
                                      &variables[cmds->set[stack_pointer].var1].data.u,
                                      &variables[cmds->set[stack_pointer].var2].data.u,
                                      &proof);
                        deallocate_geq(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_success;
                    }
                    else {
                        leq_proof_t proof;
                        allocate_leq(&proof, L1, 1);
                        int size = read_size(zk_proof);
                        uint8_t bytes[size];
                        for (int l = 0; l < size; l++) {
                            bytes[l] = fgetc(zk_proof);
                        }
                        bytes_to_rp(&proof, bytes);
                        res = leq_ver(ctx,
                                      &variables[cmds->set[stack_pointer].var1].data.u,
                                      &variables[cmds->set[stack_pointer].var2].data.u,
                                      &proof);
                        deallocate_leq(&proof);
                        if (res != 0) {
                            return -2; // error
                        }
                        stack_pointer = cmds->set[stack_pointer].next_failure;
                    }
                }
                break;
            default:
                DEBUG_PRINT(("Syntax error %d %d\n", cmds->set[stack_pointer].type, cmds->set[stack_pointer].subtype));
                FREE_LINE(line);
                return CONTRACT_SYNTAX_ERR;
        }
    }

    if (branch != 0) {
        DEBUG_PRINT(("Syntax error 3: open branches %d\n", branch));
        FREE_LINE(line);
        return CONTRACT_OPEN_BRANCHES;
    }

    if (loop != 0) {
        DEBUG_PRINT(("Syntax error 3: open loops %d\n", loop));
        FREE_LINE(line);
        return CONTRACT_OPEN_LOOPS;
    }

    return 0;
}

int parser_recursive_ver(context_t *ctx, uint8_t range,
                         char *zk_proof_path, command_set *cmds,
                         uint8_t inlen, coin_et in[],
                         uint8_t outlen, coin_et out[]) {
    FILE * zk_proof;
    var_et *variables;
    variables = (var_et *) malloc(99 * sizeof(var_et));
    int res;

    zk_proof = fopen(zk_proof_path, "r");
    if (zk_proof == nullptr) {
        DEBUG_PRINT(("can't open extra_data proof %s\n", zk_proof_path));
        exit(EXIT_FAILURE);
    }

    res = runner_ver(ctx, zk_proof, cmds, range, variables,
                     inlen, in, outlen, out);

    fclose(zk_proof);
    free(variables);

    return res;
}

