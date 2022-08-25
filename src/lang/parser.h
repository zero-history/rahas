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

#ifndef CIP_SIS_PARSER_H
#define CIP_SIS_PARSER_H

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
#include "algo.h"
#include "lang_utils.h"

#define TYPE_INT 1
#define TYPE_CINT 2
#define TYPE_PK 3

#define CONTRACT_COMP_ERR -1
#define CONTRACT_SYNTAX_ERR -100
#define CONTRACT_WHILE_TYPE_ERR -101
#define CONTRACT_MORE_END_IF_ERR -102
#define CONTRACT_LESS_END_IF_ERR -103
#define CONTRACT_ADDITIONAL_ELSE_ERR -104
#define CONTRACT_OPEN_BRANCHES -105
#define CONTRACT_WHILE_END_ERR -106
#define CONTRACT_DATA_TYPE_ERR -107
#define CONTRACT_INT_TYPE_ERR -108
#define CONTRACT_CINT_TYPE_ERR -109
#define CONTRACT_PK_TYPE_ERR -110
#define CONTRACT_OPEN_LOOPS -111


const static char *return_ok_command = "#RETURN_OK";
const static char *return_error_command = "#RETURN_ERROR";

const static char *if_end_command = "#END_IF";
const static char *else_command = "#ELSE";

const static char *set_int_command = "#SET_INT";

const static char *set_cint_command = "#SET_CINT";
const static char *set_cint_from_in_command = "#SET_CINTFIN";
const static char *set_cint_from_out_command = "#SET_CINTFOUT";
const static char *set_cint_from_in_pk_command = "#SET_CINTFINPK";
const static char *set_cint_from_out_pk_command = "#SET_CINTFOUTPK";

const static char *set_pk_command = "#SET_PK";
const static char *set_pk_from_in_command = "#SET_PKFIN";
const static char *set_pk_from_out_command = "#SET_PKFOUT";

const static char *copy_int_command = "#CPY_INT";
const static char *copy_cint_command = "#CPY_CINT";

const static char *set_int_from_inlen_command = "#SET_INTFINLEN";
const static char *set_int_from_outlen_command = "#SET_OUTFINLEN";

const static char *set_cint_from_int_command = "#SET_CINT_FROM_INT";

const static char *add_int_command = "#ADD_INT";
const static char *add_cint_command = "#ADD_CINT";

const static char *sub_int_command = "#SUB_INT";
const static char *sub_cint_command = "#SUB_CINT";

const static char *mul_int_command = "#MUL_INT";
const static char *mul_cint_command = "#MUL_CINT";

const static char *divu_cint_command = "#DIVU_CINT";

const static char *div_int_command = "#DIV_INT";
const static char *div_cint_command = "#DIV_CINT";

const static char *if_start = "#IF_";
const static size_t if_len = sizeof(char) * 4;

const static char *if_eq_int_command = "#IF_EQ_INT";
const static char *if_eq_cint_command = "#IF_EQ_CINT";
const static char *if_eq_pk_command = "#IF_EQ_PK";

const static char *if_neq_int_command = "#IF_NEQ_INT";
const static char *if_neq_cint_command = "#IF_NEQ_CINT";
const static char *if_neq_pk_command = "#IF_NEQ_PK";

const static char *if_leq_int_command = "#IF_LEQ_INT";
const static char *if_leq_cint_command = "#IF_LEQ_CINT";

const static char *if_geq_int_command = "#IF_GEQ_INT";
const static char *if_geq_cint_command = "#IF_GEQ_CINT";

const static char *while_end_command = "#END_WHILE";
const static char *while_int_eq_command  = "#WHILE_EQ_INT";
const static char *while_int_neq_command  = "#WHILE_NEQ_INT";
const static char *while_int_leq_command  = "#WHILE_LEQ_INT";
const static char *while_int_geq_command  = "#WHILE_GEQ_INT";
const static char *while_cint_eq_command  = "#WHILE_EQ_CINT";
const static char *while_cint_neq_command  = "#WHILE_NEQ_CINT";
const static char *while_cint_leq_command  = "#WHILE_LEQ_CINT";
const static char *while_cint_geq_command  = "#WHILE_GEQ_CINT";


const static unsigned char return_ok_byte[] = {0, 1};
const static unsigned char return_error_byte[] = {0, 0};

const static unsigned char set_int_byte[] = {1, 1};
const static unsigned char set_int_from_inlen_byte[] = {1, 10};
const static unsigned char set_int_from_outlen_byte[] = {1, 11};

const static unsigned char set_cint_byte[] = {1, 2};
const static unsigned char set_cint_from_in_byte[] = {1, 20};
const static unsigned char set_cint_from_out_byte[] = {1, 21};
const static unsigned char set_cint_from_in_pk_byte[] = {1, 22};
const static unsigned char set_cint_from_out_pk_byte[] = {1, 23};
const static unsigned char set_cint_from_int_byte[] = {1, 24};

const static unsigned char set_pk_byte[] = {1, 3};
const static unsigned char set_pk_from_in_byte [] = {1, 30};
const static unsigned char set_pk_from_out_byte[] = {1, 31};

const static unsigned char copy_int_byte[] = {2, 0};
const static unsigned char copy_cint_byte[] = {2, 1};

const static unsigned char add_int_byte[] = {3, 10};
const static unsigned char add_cint_byte[] = {3, 20};

const static unsigned char sub_int_byte[] = {3, 11};
const static unsigned char sub_cint_byte[] = {3, 21};

const static unsigned char mul_int_byte[] = {3, 12};
const static unsigned char mul_cint_byte[] = {3, 22};

const static unsigned char div_int_byte[] = {3, 13};
const static unsigned char div_cint_byte[] = {3, 23};
const static unsigned char divu_cint_byte[] = {3, 230};

const static unsigned char if_end_byte[] = {4, 0};
const static unsigned char else_byte[] = {4, 1};
const static unsigned char if_eq_int_byte[] = {4, 11};
const static unsigned char if_neq_int_byte[] = {4, 12};
const static unsigned char if_leq_int_byte[] = {4, 13};
const static unsigned char if_geq_int_byte[] = {4, 14};
const static unsigned char if_eq_cint_byte[] = {4, 21};
const static unsigned char if_neq_cint_byte[] = {4, 22};
const static unsigned char if_leq_cint_byte[] = {4, 23};
const static unsigned char if_geq_cint_byte[] = {4, 24};
const static unsigned char if_eq_pk_byte[] = {4, 31};
const static unsigned char if_neq_pk_byte[] = {4, 32};

const static unsigned char while_end_byte[] = {5, 0};
const static unsigned char while_int_eq_byte[] = {5, 11};
const static unsigned char while_int_neq_byte[] = {5, 12};
const static unsigned char while_int_leq_byte[] = {5, 13};
const static unsigned char while_int_geq_byte[] = {5, 14};
const static unsigned char while_cint_eq_byte[] = {5, 21};
const static unsigned char while_cint_neq_byte[] = {5, 22};
const static unsigned char while_cint_leq_byte[] = {5, 23};
const static unsigned char while_cint_geq_byte[] = {5, 24};

typedef struct command_struct {
    uint8_t type = -1;
    uint8_t subtype = -1;
    int16_t var1 = -1;
    int16_t var2 = -1;
    int16_t var3 = -1;
    int16_t var4 = -1;
    int16_t next_success = -1;
    int16_t next_failure = -1;
    uint8_t data_type;
    int64_t v;
    com_t u; // To reduce the memory consumption
    uint8_t pk[PKID];
} command_t;

typedef struct command_set_struct {
    command_t *set;
} command_set;

void print_command_struct(command_t *cmd); // For debugging
int commands_to_file(int16_t line_no, command_set *cmds, FILE *code_file, uint8_t L1);
int command_from_file(command_set *cmds, FILE *code_file, int16_t line_no, uint8_t L1);

int get_byte_code(command_t *cmds, int16_t line_no, char *code, var_ct extra[]) ;

int pre_runner(command_set *cmds, FILE *fp, var_ct *extra);

int runner_gen(context_t *ctx, FILE *zk_proof, command_set *cmds,
               uint8_t L1, var_ct *variables, uint8_t  extralen, var_ct *extra,
               uint8_t inlen, coin_ct in[],
               uint8_t outlen, coin_ct out[]);

int runner_ver(context_t *ctx, FILE *zk_proof, command_set *cmds,
               uint8_t L1, var_et *variables,
               uint8_t inlen, coin_et in[],
               uint8_t outlen, coin_et out[]);



#endif //CIP_SIS_PARSER_H
