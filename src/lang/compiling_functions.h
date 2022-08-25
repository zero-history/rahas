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


#ifndef CIP_SIS_LANG_COMPILING
#define CIP_SIS_LANG_COMPILING


int set_int_gen(command_t *cmd, var_ct *variables) {
    if (cmd->data_type != TYPE_INT) {
        DEBUG_PRINT(("int type error in set_int_gen: %d %d\n", cmd->var2, cmd->data_type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].type = TYPE_INT;
    variables[cmd->var1].data.v = cmd->v;
    return 0;
}

void set_int_from_lenths_gen(command_t *cmd, var_ct *variables, int v) {
    variables[cmd->var1].type = TYPE_INT;
    variables[cmd->var1].data.v = v;
}

int set_cint_gen(command_t *cmd, var_ct *variables, var_ct *extra) {
    if (extra[cmd->var2].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in set_cint_gen: %d %d\n", cmd->var2, extra[cmd->var2].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    var_ct_copy(&variables[cmd->var1].data, &extra[cmd->var2].data);
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}

void set_cint_from_in_gen(command_t *cmd, var_ct *variables, coin_ct *in) {
    var_ct_copy(&variables[cmd->var1].data, &in[cmd->var2].c);
    variables[cmd->var1].type = TYPE_CINT;
}

void set_cint_from_out_gen(command_t *cmd, var_ct *variables, coin_ct *out) {
    var_ct_copy(&variables[cmd->var1].data, &out[cmd->var2].c);
    variables[cmd->var1].type = TYPE_CINT;
}

int set_cint_from_in_pk_gen(command_t *cmd, var_ct *variables, uint16_t inlen, coin_ct *in) {
    if (variables[cmd->var2].type != TYPE_PK) {
        DEBUG_PRINT(("pk type error in set_cint_from_in_pk_gen: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_PK_TYPE_ERR;
    }
    int found = 0;
    for (int l = 0; l < inlen; l++) {
        if (memcmp(variables[cmd->var2].data.pk, in[l].c.pk, PKID) == 0) {
            var_ct_copy(&variables[cmd->var1].data, &in[l].c);
            variables[cmd->var1].data.is_set_pk = 0;
            variables[cmd->var1].type = TYPE_CINT;
            found = 1;
            DEBUG_PRINT(("pk %d\n", l));
            break;
        }
    }
    if (found == 0) {
        for (auto & l : variables[cmd->var1].data.k.k) {
            poly_set_zero(&l, 0, N);
        }
        poly_n_set_zero(&variables[cmd->var1].data.u, 0, N);
        variables[cmd->var1].data.v = 0;
        variables[cmd->var1].data.is_set_pk = 0;
        variables[cmd->var1].type = TYPE_CINT;
        DEBUG_PRINT(("pk not found\n"));
    }
    return 0;
}

int set_cint_from_out_pk_gen(command_t *cmd, var_ct *variables, uint16_t outlen, coin_ct *out) {
    if (variables[cmd->var2].type != TYPE_PK) {
        DEBUG_PRINT(("pk type error in set_cint_from_out_pk_gen: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_PK_TYPE_ERR;
    }
    int found = 0;
    for (int l = 0; l < outlen; l++) {
        if (memcmp(variables[cmd->var2].data.pk, out[l].c.pk, PKID) == 0) {
            var_ct_copy(&variables[cmd->var1].data, &out[l].c);
            variables[cmd->var1].data.is_set_pk = 0;
            variables[cmd->var1].type = TYPE_CINT;
            found = 1;
            DEBUG_PRINT(("pk %d\n", l));
            break;
        }
    }
    if (found == 0) {
        for (auto & l : variables[cmd->var1].data.k.k) {
            poly_set_zero(&l, 0, N);
        }
        poly_n_set_zero(&variables[cmd->var1].data.u, 0, N);
        variables[cmd->var1].data.v = 0;
        variables[cmd->var1].data.is_set_pk = 0;
        variables[cmd->var1].type = TYPE_CINT;
        DEBUG_PRINT(("pk not found\n"));
    }
    return 0;
}

int set_pk_gen(command_t *cmd, var_ct *variables, var_ct *extra) {
    if (extra[cmd->var2].type != TYPE_PK) {
        DEBUG_PRINT(("pk type error in set_pk_gen: %d %d\n", cmd->var2, extra[cmd->var2].type));
        return CONTRACT_PK_TYPE_ERR;
    }
    var_pk_copy(&variables[cmd->var1], &extra[cmd->var2].data);
    variables[cmd->var1].type = TYPE_PK;
    return 0;
}

void set_pk_from_in_gen(command_t *cmd, var_ct *variables, coin_ct *in) {
    var_pk_copy(&variables[cmd->var1], &in[cmd->var2].c);
    variables[cmd->var1].type = TYPE_PK;
}

void set_pk_from_out_gen(command_t *cmd, var_ct *variables, coin_ct *out) {
    var_pk_copy(&variables[cmd->var1], &out[cmd->var2].c);
    variables[cmd->var1].type = TYPE_PK;
}

int copy_int_gen(command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in copy_int_gen: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v;
    variables[cmd->var1].type = TYPE_INT;
    return 0;
}

int copy_cint_gen(command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in copy_int_gen: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    var_ct_copy(&variables[cmd->var1].data, &variables[cmd->var2].data);
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}

int set_cint_from_int_gen(context_t *ctx, command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in set_cint_from_int_gen: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v;
    for (int i = 0; i < mk; i++) {
        poly_set_zero(&variables[cmd->var1].data.k.k[i], 0, N);
    }
    variables[cmd->var1].data.is_set_pk = 0;
    commit_from_int(ctx, 1, &variables[cmd->var1].data.u, variables[cmd->var1].data.k.k, &variables[cmd->var1].data.v);
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}

int while_int_neq_gen(command_t *cmd, var_ct *variables) {
    if (variables[cmd->var1].type != variables[cmd->var2].type)
        return CONTRACT_WHILE_TYPE_ERR;
    return (variables[cmd->var1].data.v != variables[cmd->var2].data.v);
}

int add_int_gen(command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_INT ||
        variables[cmd->var3].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in add_int_gen: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v + variables[cmd->var3].data.v;
    variables[cmd->var1].type = TYPE_INT;
    return 0;
}

int sub_int_gen(command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_INT ||
        variables[cmd->var3].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in sub_int_gen: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v - variables[cmd->var3].data.v;
    variables[cmd->var1].type = TYPE_INT;
    return 0;
}

int mul_int_gen(command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_INT ||
        variables[cmd->var3].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in mul_int_gen: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v * variables[cmd->var3].data.v;
    variables[cmd->var1].type = TYPE_INT;
    return 0;
}

int div_int_gen(command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_INT ||
        variables[cmd->var3].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in div_int_gen: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v / variables[cmd->var3].data.v;
    variables[cmd->var4].data.v = variables[cmd->var2].data.v % variables[cmd->var3].data.v;
    variables[cmd->var1].type = TYPE_INT;
    variables[cmd->var4].type = TYPE_INT;
    return 0;
}

int add_cint_gen(context_t *ctx, uint8_t L1, command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_CINT ||
        variables[cmd->var3].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in add_cint_gen: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    add_gen(ctx, 1,
            &variables[cmd->var1].data.u, &variables[cmd->var1].data.v, &variables[cmd->var1].data.k,
            &variables[cmd->var2].data.u, &variables[cmd->var2].data.v, &variables[cmd->var2].data.k,
            &variables[cmd->var3].data.u, &variables[cmd->var3].data.v, &variables[cmd->var3].data.k);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}


int sub_cint_gen(context_t *ctx, uint8_t L1, command_t *cmd, var_ct *variables) {
    if (variables[cmd->var2].type != TYPE_CINT ||
        variables[cmd->var3].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in sub_cint_gen: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    sub_gen(ctx, 1,
            &variables[cmd->var1].data.u, &variables[cmd->var1].data.v, &variables[cmd->var1].data.k,
            &variables[cmd->var2].data.u, &variables[cmd->var2].data.v, &variables[cmd->var2].data.k,
            &variables[cmd->var3].data.u, &variables[cmd->var3].data.v, &variables[cmd->var3].data.k);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}


int mul_cint_gen(context_t *ctx, uint8_t L1, command_t *cmd, var_ct *variables, FILE *zk_proof) {
    if (variables[cmd->var2].type != TYPE_CINT ||
        variables[cmd->var3].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in mul_cint_gen: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    mul_pt proof;
    allocate_mul_proof(&proof, L1, 1);
    mul_gen(ctx, L1, 1, &proof,
            &variables[cmd->var1].data.u, &variables[cmd->var1].data.v, &variables[cmd->var1].data.k,
            &variables[cmd->var2].data.u, &variables[cmd->var2].data.v, &variables[cmd->var2].data.k,
            &variables[cmd->var3].data.u, &variables[cmd->var3].data.v, &variables[cmd->var3].data.k);

    // Store u0
    uint8_t u0_bytes[COM_BYTES];
    com_to_bytes(u0_bytes, &variables[cmd->var1].data.u);
    for (int l = 0; l < COM_BYTES; l++) {
        fputc(u0_bytes[l], zk_proof);
    }

    // Store proof
    int size = get_mul_pt_size(&proof);
    store_size(zk_proof, size);
    uint8_t bytes[size];
    mul_pt_to_bytes(bytes, &proof);
    for (int l = 0; l < size; l++) {
        fputc(bytes[l], zk_proof);
    }
    deallocate_mul_proof(&proof);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}


int divu_cint_gen(context_t *ctx, uint8_t L1, command_t *cmd, var_ct *variables, FILE *zk_proof) {
    if (variables[cmd->var3].type != TYPE_CINT ||
        variables[cmd->var4].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in divu_cint_gen: %d %d\n", variables[cmd->var3].type, variables[cmd->var4].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    div_upt proof;
    allocate_div_unsigned_proof(&proof, L1, 1);
    div_unsigned_gen(ctx, L1, 1, &proof,
                   &variables[cmd->var1].data.u, &variables[cmd->var1].data.v, &variables[cmd->var1].data.k,
                   &variables[cmd->var2].data.u, &variables[cmd->var2].data.v, &variables[cmd->var2].data.k,
                   &variables[cmd->var3].data.u, &variables[cmd->var3].data.v, &variables[cmd->var3].data.k,
                   &variables[cmd->var4].data.u, &variables[cmd->var4].data.v, &variables[cmd->var4].data.k);

    // Store u0 and u3
    uint8_t u_bytes[COM_BYTES];
    com_to_bytes(u_bytes, &variables[cmd->var1].data.u);
    for (int l = 0; l < COM_BYTES; l++) {
        fputc(u_bytes[l], zk_proof);
    }
    com_to_bytes(u_bytes, &variables[cmd->var2].data.u);
    for (int l = 0; l < COM_BYTES; l++) {
        fputc(u_bytes[l], zk_proof);
    }

    // Store proof
    int size = get_div_upt_size(&proof);
    store_size(zk_proof, size);
    uint8_t bytes[size];
    div_upt_to_bytes(bytes, &proof);
    for (int l = 0; l < size; l++) {
        fputc(bytes[l], zk_proof);
    }
    deallocate_div_unsigned_proof(&proof);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    variables[cmd->var2].data.is_set_pk = 0;
    variables[cmd->var2].type = TYPE_CINT;
    return 0;
}

int div_cint_gen(context_t *ctx, uint8_t L1, command_t *cmd, var_ct *variables, FILE *zk_proof) {
    if (variables[cmd->var3].type != TYPE_CINT ||
        variables[cmd->var4].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in div_cint_gen: %d %d\n", variables[cmd->var3].type, variables[cmd->var4].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    div_pt proof;
    allocate_div_signed_proof(&proof, L1, 1);
    div_signed_gen(ctx, L1, 1, &proof,
                     &variables[cmd->var1].data.u, &variables[cmd->var1].data.v, &variables[cmd->var1].data.k,
                     &variables[cmd->var2].data.u, &variables[cmd->var2].data.v, &variables[cmd->var2].data.k,
                     &variables[cmd->var3].data.u, &variables[cmd->var3].data.v, &variables[cmd->var3].data.k,
                     &variables[cmd->var4].data.u, &variables[cmd->var4].data.v, &variables[cmd->var4].data.k);

    // Store u0 and u3
    uint8_t u_bytes[COM_BYTES];
    com_to_bytes(u_bytes, &variables[cmd->var1].data.u);
    for (int l = 0; l < COM_BYTES; l++) {
        fputc(u_bytes[l], zk_proof);
    }
    com_to_bytes(u_bytes, &variables[cmd->var2].data.u);
    for (int l = 0; l < COM_BYTES; l++) {
        fputc(u_bytes[l], zk_proof);
    }

    // Store proof
    int size = get_div_pt_size(&proof);
    store_size(zk_proof, size);
    uint8_t bytes[size];
    div_pt_to_bytes(bytes, &proof);
    for (int l = 0; l < size; l++) {
        fputc(bytes[l], zk_proof);
    }
    deallocate_div_signed_proof(&proof);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    variables[cmd->var2].data.is_set_pk = 0;
    variables[cmd->var2].type = TYPE_CINT;
    return 0;
}

#endif //CIP_SIS_LANG_COMPILING