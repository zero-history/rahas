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

#ifndef CIP_SIS_LANG_EXECUTION
#define CIP_SIS_LANG_EXECUTION

int set_int_ver(command_t *cmd, var_et *variables) {
    if (cmd->data_type != TYPE_INT) {
        DEBUG_PRINT(("int type error in set_int_ver: %d %d\n", cmd->var2, cmd->data_type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].type = TYPE_INT;
    variables[cmd->var1].data.v = cmd->v;
    return 0;
}

void set_int_from_lenths_ver(command_t *cmd, var_et *variables, int v) {
    variables[cmd->var1].type = TYPE_INT;
    variables[cmd->var1].data.v = v;
}

int set_cint_ver(command_t *cmd, var_et *variables) {
    if (cmd->data_type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in set_cint_ver: %d %d\n", cmd->var2, cmd->data_type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    variables[cmd->var1].type = TYPE_CINT;
    poly_n_set(&variables[cmd->var1].data.u, &cmd->u);
    return 0;
}

void set_cint_from_in_ver(command_t *cmd, var_et *variables, coin_et *in) {
    var_et_copy(&variables[cmd->var1].data, &in[cmd->var2].c);
    variables[cmd->var1].type = TYPE_CINT;
}

void set_cint_from_out_ver(command_t *cmd, var_et *variables, coin_et *out) {
    var_et_copy(&variables[cmd->var1].data, &out[cmd->var2].c);
    variables[cmd->var1].type = TYPE_CINT;
}

int set_cint_from_in_pk_ver(command_t *cmd, var_et *variables, uint16_t inlen, coin_et *in) {
    if (variables[cmd->var2].type != TYPE_PK) {
        DEBUG_PRINT(("pk type error in set_cint_from_in_pk_ver: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_PK_TYPE_ERR;
    }
    int found = 0;
    for (int l = 0; l < inlen; l++) {
        if (memcmp(variables[cmd->var2].data.pk, in[l].c.pk, PKID) == 0) {
            var_et_copy(&variables[cmd->var1].data, &in[l].c);
            variables[cmd->var1].data.is_set_pk = 0;
            variables[cmd->var1].type = TYPE_CINT;
            found = 1;
            break;
        }
    }
    if (found == 0) {
        poly_n_set_zero(&variables[cmd->var1].data.u, 0, N);
        variables[cmd->var1].data.is_set_pk = 0;
        variables[cmd->var1].type = TYPE_CINT;
    }
    return 0;
}

int set_cint_from_out_pk_ver(command_t *cmd, var_et *variables, uint16_t outlen, coin_et *out) {
    if (variables[cmd->var2].type != TYPE_PK) {
        DEBUG_PRINT(("pk type error in set_cint_from_out_pk_ver: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_PK_TYPE_ERR;
    }
    int found = 0;
    for (int l = 0; l < outlen; l++) {
        if (memcmp(variables[cmd->var2].data.pk, out[l].c.pk, PKID) == 0) {
            var_et_copy(&variables[cmd->var1].data, &out[l].c);
            variables[cmd->var1].data.is_set_pk = 0;
            variables[cmd->var1].type = TYPE_CINT;
            found = 1;
            break;
        }
    }
    if (found == 0) {
        poly_n_set_zero(&variables[cmd->var1].data.u, 0, N);
        variables[cmd->var1].data.is_set_pk = 0;
        variables[cmd->var1].type = TYPE_CINT;
    }
    return 0;
}

int set_pk_ver(command_t *cmd, var_et *variables) {
    if (cmd->data_type != TYPE_PK) {
        DEBUG_PRINT(("pk type error in set_pk_ver: %d %d\n", cmd->var2, cmd->data_type));
        return CONTRACT_PK_TYPE_ERR;
    }
    memcpy(variables[cmd->var1].data.pk, cmd->pk, PKID);
    variables[cmd->var1].data.is_set_pk = 1;
    variables[cmd->var1].type = TYPE_PK;
    return 0;
}

void set_pk_from_in_ver(command_t *cmd, var_et *variables, coin_et *in) {
    var_pk_et_copy(&variables[cmd->var1], &in[cmd->var2].c);
    variables[cmd->var1].type = TYPE_PK;
}

void set_pk_from_out_ver(command_t *cmd, var_et *variables, coin_et *out) {
    var_pk_et_copy(&variables[cmd->var1], &out[cmd->var2].c);
    variables[cmd->var1].type = TYPE_PK;
}

int copy_int_ver(command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in copy_int_ver: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v;
    variables[cmd->var1].type = TYPE_INT;
    return 0;
}

int copy_cint_ver(command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in copy_int_ver: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    var_et_copy(&variables[cmd->var1].data, &variables[cmd->var2].data);
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}

int set_cint_from_int_ver(context_t *ctx, command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in set_cint_from_int_ver: %d %d\n", cmd->var2, variables[cmd->var2].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.is_set_pk = 0;
    commit_from_int(ctx, 1, &variables[cmd->var1].data.u, nullptr, &variables[cmd->var2].data.v);
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}

int add_int_ver(command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_INT ||
        variables[cmd->var3].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in add_int_ver: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v + variables[cmd->var3].data.v;
    variables[cmd->var1].type = TYPE_INT;
    return 0;
}

int sub_int_ver(command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_INT ||
        variables[cmd->var3].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in sub_int_ver: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v - variables[cmd->var3].data.v;
    variables[cmd->var1].type = TYPE_INT;
    return 0;
}

int mul_int_ver(command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_INT ||
        variables[cmd->var3].type != TYPE_INT) {
        DEBUG_PRINT(("int type error in mul_int_ver: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v * variables[cmd->var3].data.v;
    variables[cmd->var1].type = TYPE_INT;
    return 0;
}

int div_int_ver(command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_INT ||
        variables[cmd->var3].type != TYPE_INT) {
        DEBUG_PRINT(("cint type error in div_int_ver: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_INT_TYPE_ERR;
    }
    variables[cmd->var1].data.v = variables[cmd->var2].data.v / variables[cmd->var3].data.v;
    variables[cmd->var4].data.v = variables[cmd->var2].data.v % variables[cmd->var3].data.v;
    variables[cmd->var1].type = TYPE_INT;
    variables[cmd->var4].type = TYPE_INT;
    return 0;
}

int add_cint_ver(context_t *ctx, uint8_t L1, command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_CINT ||
        variables[cmd->var3].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in add_cint_ver: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    poly_n_add(&variables[cmd->var1].data.u, &variables[cmd->var2].data.u, &variables[cmd->var3].data.u);
    poly_n_reduce(&variables[cmd->var1].data.u);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}

int sub_cint_ver(context_t *ctx, uint8_t L1, command_t *cmd, var_et *variables) {
    if (variables[cmd->var2].type != TYPE_CINT ||
        variables[cmd->var3].type != TYPE_CINT) {
        DEBUG_PRINT(("int type error in sub_cint_ver: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    poly_n_sub(&variables[cmd->var1].data.u, &variables[cmd->var2].data.u, &variables[cmd->var3].data.u);
    poly_n_reduce(&variables[cmd->var1].data.u);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    return 0;
}

int mul_cint_ver(context_t *ctx, uint8_t L1, command_t *cmd, var_et *variables, FILE *zk_proof) {
    if (variables[cmd->var2].type != TYPE_CINT ||
        variables[cmd->var3].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in mul_cint_ver: %d %d\n", variables[cmd->var2].type, variables[cmd->var3].type));
        return CONTRACT_CINT_TYPE_ERR;
    }

    // Read u0
    uint8_t u0_bytes[COM_BYTES];
    for (int l = 0; l < COM_BYTES; l++) {
        u0_bytes[l] = fgetc(zk_proof);
    }
    bytes_to_com(&variables[cmd->var1].data.u, u0_bytes);

    // Read proof
    mul_pt proof;
    allocate_mul_proof(&proof, L1, 1);
    int size = read_size(zk_proof);
    uint8_t bytes[size];
    for (int l = 0; l < size; l++) {
        bytes[l] = fgetc(zk_proof);
    }
    bytes_to_mul_pt(&proof, bytes);
    int res = mul_ver(ctx, &variables[cmd->var1].data.u,
                      &variables[cmd->var2].data.u,
                      &variables[cmd->var3].data.u, &proof);
    deallocate_mul_proof(&proof);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    if (res != 0)
        return -2; // error
    return 0;
}


int divu_cint_ver(context_t *ctx, uint8_t L1, command_t *cmd, var_et *variables, FILE *zk_proof) {
    if (variables[cmd->var3].type != TYPE_CINT ||
        variables[cmd->var4].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in divu_cint_ver: %d %d\n", variables[cmd->var3].type, variables[cmd->var4].type));
        return CONTRACT_CINT_TYPE_ERR;
    }

    // Read u0 and u3
    uint8_t u_bytes[COM_BYTES];
    for (int l = 0; l < COM_BYTES; l++) {
        u_bytes[l] = fgetc(zk_proof);
    }
    bytes_to_com(&variables[cmd->var1].data.u, u_bytes);

    for (int l = 0; l < COM_BYTES; l++) {
        u_bytes[l] = fgetc(zk_proof);
    }
    bytes_to_com(&variables[cmd->var2].data.u, u_bytes);

    // Read proof
    div_upt proof;
    allocate_div_unsigned_proof(&proof, L1, 1);
    int size = read_size(zk_proof);
    uint8_t bytes[size];
    for (int l = 0; l < size; l++) {
        bytes[l] = fgetc(zk_proof);
    }
    bytes_to_div_upt(&proof, bytes);
    int res = div_unsigned_ver(ctx, &variables[cmd->var1].data.u,
                               &variables[cmd->var3].data.u,
                               &variables[cmd->var4].data.u,
                               &variables[cmd->var2].data.u, &proof);
    deallocate_div_unsigned_proof(&proof);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    variables[cmd->var2].data.is_set_pk = 0;
    variables[cmd->var2].type = TYPE_CINT;
    if (res != 0)
        return -2; // error
    return 0;
}

int div_cint_ver(context_t *ctx, uint8_t L1, command_t *cmd, var_et *variables, FILE *zk_proof) {
    if (variables[cmd->var3].type != TYPE_CINT ||
        variables[cmd->var4].type != TYPE_CINT) {
        DEBUG_PRINT(("cint type error in div_cint_ver: %d %d\n", variables[cmd->var3].type, variables[cmd->var4].type));
        return CONTRACT_CINT_TYPE_ERR;
    }
    // Read u0 and u3
    uint8_t u_bytes[COM_BYTES];
    for (int l = 0; l < COM_BYTES; l++) {
        u_bytes[l] = fgetc(zk_proof);
    }
    bytes_to_com(&variables[cmd->var1].data.u, u_bytes);

    for (int l = 0; l < COM_BYTES; l++) {
        u_bytes[l] = fgetc(zk_proof);
    }
    bytes_to_com(&variables[cmd->var2].data.u, u_bytes);

    // Read proof
    div_pt proof;
    allocate_div_signed_proof(&proof, L1, 1);
    int size = read_size(zk_proof);
    uint8_t bytes[size];
    for (int l = 0; l < size; l++) {
        bytes[l] = fgetc(zk_proof);
    }
    bytes_to_div_pt(&proof, bytes);
    int res = div_signed_ver(ctx, &variables[cmd->var1].data.u,
                               &variables[cmd->var3].data.u,
                               &variables[cmd->var4].data.u,
                               &variables[cmd->var2].data.u, &proof);
    deallocate_div_signed_proof(&proof);
    variables[cmd->var1].data.is_set_pk = 0;
    variables[cmd->var1].type = TYPE_CINT;
    variables[cmd->var2].data.is_set_pk = 0;
    variables[cmd->var2].type = TYPE_CINT;
    if (res != 0)
        return -2; // error
    return 0;
}

#endif //CIP_SIS_LANG_EXECUTION