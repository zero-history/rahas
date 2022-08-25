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


int commands_to_file(int16_t line_no, command_set *cmds, FILE *code_file, uint8_t L1) {
    fputc(line_no, code_file);
    fputc(L1, code_file);
    int64_t v;
    uint8_t l1;
    uint8_t bytes[COM_BYTES];
    int i, j;
    for (i = 0; i < line_no; i++) {
        fputc(cmds->set[i].type, code_file);
        fputc(cmds->set[i].subtype, code_file);
        fputc((cmds->set[i].var1 & 0xFF00) >> 8, code_file);
        fputc(cmds->set[i].var1 & 0xFF, code_file);
        fputc((cmds->set[i].var2 & 0xFF00) >> 8, code_file);
        fputc(cmds->set[i].var2 & 0xFF, code_file);
        fputc((cmds->set[i].var3 & 0xFF00) >> 8, code_file);
        fputc(cmds->set[i].var3 & 0xFF, code_file);
        fputc((cmds->set[i].var4 & 0xFF00) >> 8, code_file);
        fputc(cmds->set[i].var4 & 0xFF, code_file);
        fputc(cmds->set[i].next_success & 0xFF, code_file);
        fputc((cmds->set[i].next_success >> 8) & 0xFF, code_file);
        fputc(cmds->set[i].next_failure & 0xFF, code_file);
        fputc((cmds->set[i].next_failure >> 8) & 0xFF, code_file);
        if (cmds->set[i].type == 1 && (cmds->set[i].subtype == 1 || cmds->set[i].subtype == 2 || cmds->set[i].subtype == 3)) {
            fputc(cmds->set[i].data_type, code_file);
            com_t u;
            switch (cmds->set[i].data_type) {
                case 0:
                    l1 = 0;
                    v = cmds->set[i].v;
                    while (l1 <= L1) {
                        fputc((uint8_t) (v & 0xFF), code_file);
                        v >>= 8;
                        l1 += 8;
                    }
                    break;
                case 1:
                    com_to_bytes(bytes, &cmds->set[i].u);
                    for (j = 0; j < COM_BYTES; j++)
                        fputc(bytes[j], code_file);
                    break;
                case 2:
                    for (j = 0; j < PKID; j++) {
                        fputc(cmds->set[i].pk[j], code_file);
                    }
                    break;
                default:
                    DEBUG_PRINT(("Syntax error in external %d of data type %d "\
                                 "while trying assigning to variable %d\n", i, cmds->set[i].var1, cmds->set[i].data_type));
                    return CONTRACT_DATA_TYPE_ERR;
                    break;
            }
        }
    }
    return 0;
}

int command_from_file(command_set *cmds, FILE *code_file, int16_t line_no, uint8_t L1) {
    uint8_t l1;
    uint8_t bytes[COM_BYTES];
    int i, j;
    for (i = 0; i < line_no; i++) {
        cmds->set[i].type = fgetc(code_file);
        cmds->set[i].subtype = fgetc(code_file);
        cmds->set[i].var1 = (int16_t)fgetc(code_file);
        cmds->set[i].var1 <<= 8;
        cmds->set[i].var1 += (int16_t) fgetc(code_file);
        cmds->set[i].var2 = (int16_t)fgetc(code_file);
        cmds->set[i].var2 <<= 8;
        cmds->set[i].var2 += (int16_t) fgetc(code_file);
        cmds->set[i].var3 = (int16_t)fgetc(code_file);
        cmds->set[i].var3 <<= 8;
        cmds->set[i].var3 += (int16_t) fgetc(code_file);
        cmds->set[i].var4 = (int16_t)fgetc(code_file);
        cmds->set[i].var4 <<= 8;
        cmds->set[i].var4 += (int16_t) fgetc(code_file);
        cmds->set[i].next_success = (int16_t)fgetc(code_file);
        cmds->set[i].next_success |= (int16_t)(fgetc(code_file) << 8);
        cmds->set[i].next_failure = (int16_t)fgetc(code_file);
        cmds->set[i].next_failure |= (int16_t)(fgetc(code_file) << 8);
        if (cmds->set[i].type == 1 && (cmds->set[i].subtype == 1 || cmds->set[i].subtype == 2 || cmds->set[i].subtype == 3)) {
            cmds->set[i].data_type = fgetc(code_file);
            switch (cmds->set[i].data_type) {
                case 0:
                    l1 = 0;
                    while (l1 <= L1) {
                        cmds->set[i].v += (fgetc(code_file) << l1);
                        l1 += 8;
                    }
                    break;
                case 1:
                    for (j = 0; j < COM_BYTES; j++)
                        bytes[j] = fgetc(code_file);
                    bytes_to_com(&cmds->set[i].u, bytes);
                    break;
                case 2:
                    for (j = 0; j < PKID; j++)
                        cmds->set[i].pk[j] = fgetc(code_file);
                    break;
                default:
                    DEBUG_PRINT(("Syntax error in external %d of data type %d "\
                                 "while trying assigning to variable %d\n", i, cmds->set[i].var1, cmds->set[i].data_type));
                    return CONTRACT_DATA_TYPE_ERR;
                    break;
            }
        }
    }
    return 0;
}

// 0 - program begin
// 1 - if/else
int pre_runner(command_set *cmds, FILE *fp, var_ct *extra) {
    char *line = nullptr;
    size_t len = 0;
    ssize_t read;
    size_t if_len = sizeof(char)*4;
    size_t total = 0;
    int16_t lines = 0;
    char *code;

    // Set externals and read file into a string
    while ((read = getline(&line, &len, fp)) != -1) {

        DEBUG_PRINT(("\nRetrieved line of length %zu:",  read));
        DEBUG_PRINT(("Found if %d\n", strncmp(line, "#IF_", if_len) == 0));
        DEBUG_PRINT(("%s", line));

        total += (read + 1);
        if (lines == 0) {
            code = (char *) malloc(total);
            strcpy(code, line);
        }
        else {
            code = (char *) realloc(code, total);
            strcat(code, line);
        }
        char *token = strtok(line, " \t");
        if (strcmp(token, "\n") == 0) {
            continue;
        }
        lines++;
    }
    FREE_LINE(line);

    cmds->set = (command_t*) malloc(lines * sizeof(command_t));
    int res = get_byte_code(cmds->set, lines, code, extra);
    if (res != 1) {
        free(cmds->set);
        DEBUG_PRINT(("Error occurred in parser\n"));
    }

    FREE_LINE(code);
    return res;
}

int pre_parser(command_set *cmds, char *zksc_contract_path, var_ct extra[]) {
    FILE * fp;
    int res;

    fp = fopen(zksc_contract_path, "r");
    if (fp == nullptr) {
        DEBUG_PRINT(("can't create fp proof %s\n", zksc_contract_path));
        exit(EXIT_FAILURE);
    }

    res = pre_runner(cmds, fp, extra);

    fclose(fp);

    return res;
}
