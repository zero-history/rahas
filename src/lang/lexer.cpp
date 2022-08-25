//
// Created by jayamine on 8/14/22.
//
#include "parser.h"

void pre_set_int_gen(command_t *cmd, char token[]) {
    token = strtok(nullptr, " \t");
    uint8_t j = get_variable(token);
    DEBUG_PRINT(("out %d\n", j));
    token = strtok(nullptr, " \t");
    cmd->var1 = j;
    cmd->var2 = -1;
    cmd->var3 = -1;
    cmd->var4 = -1;
    cmd->v = atoi(token);
    cmd->data_type = TYPE_INT;
    DEBUG_PRINT(("in %ld\n", cmd->v));
}

void pre_set_cint_gen(command_t *cmd, char token[], var_ct *extra) {
    token = strtok(nullptr, " \t");
    uint8_t j = get_variable(token);
    DEBUG_PRINT(("out %d\n", j));
    token = strtok(nullptr, " \t\n");
    uint8_t i = get_extravariable(token);
    DEBUG_PRINT(("in %d\n", i));
    cmd->var1 = j;
    cmd->var2 = i;
    cmd->var3 = -1;
    cmd->var4 = -1;
    cmd->data_type = TYPE_CINT;
    poly_n_set(&cmd->u, &extra[cmd->var2].data.u);
}

void pre_set_pk_gen(command_t *cmd, char token[], var_ct *extra) {
    token = strtok(nullptr, " \t");
    uint8_t j = get_variable(token);
    DEBUG_PRINT(("out %d\n", j));
    token = strtok(nullptr, " \t\n");
    uint8_t i = get_extravariable(token);
    DEBUG_PRINT(("in %d\n", i));
    cmd->var1 = j;
    cmd->var2 = i;
    cmd->var3 = -1;
    cmd->var4 = -1;
    cmd->data_type = TYPE_PK;
    memcpy(cmd->pk, extra[cmd->var2].data.pk, PKID);
}

void set_command(command_t *cmd, const unsigned char *command,
                 int16_t next_success, int16_t next_failure) {
    cmd->type = command[0];
    cmd->subtype = command[1];
    cmd->next_success = next_success;
    cmd->next_failure = next_failure;
    cmd->data_type = -1;
    cmd->var1 = -1;
    cmd->var2 = -1;
    cmd->var3 = -1;
    cmd->var4 = -1;
}

void set_1variable(command_t *cmd, char token[]) {
    token = strtok(nullptr, " \t");
    cmd->var1 = (int16_t) get_variable(token);
    cmd->var2 = -1;
    cmd->var3 = -1;
    cmd->var4 = -1;
}

void set_2variables(command_t *cmd, char token[]) {
    token = strtok(nullptr, " \t");
    cmd->var1 = (int16_t) get_variable(token);
    token = strtok(nullptr, " \t\n");
    cmd->var2 = (int16_t) get_variable(token);
    cmd->var3 = -1;
    cmd->var4 = -1;
}

void set_3variables(command_t *cmd, char token[]) {
    token = strtok(nullptr, " \t");
    cmd->var1 = (int16_t) get_variable(token);
    token = strtok(nullptr, " \t");
    cmd->var2 = (int16_t) get_variable(token);
    token = strtok(nullptr, " \t\n");
    cmd->var3 = (int16_t) get_variable(token);
    cmd->var4 = -1;
}

void set_4variables(command_t *cmd, char token[]) {
    token = strtok(nullptr, " \t");
    cmd->var1 = (int16_t) get_variable(token);
    token = strtok(nullptr, " \t");
    cmd->var2 = (int16_t) get_variable(token);
    token = strtok(nullptr, " \t");
    cmd->var3 = (int16_t) get_variable(token);
    token = strtok(nullptr, " \t\n");
    cmd->var4 = (int16_t) get_variable(token);
}

int16_t find_corresponding_if(command_t *cmds, int16_t current_point) {
    int16_t opened = -1;
    current_point--;
    while (opened <= -1 && current_point >= 0) {
        if (cmds[current_point].type == 4 && cmds[current_point].subtype != else_byte[1]) {
            opened += ((int16_t) (1 - 2*(cmds[current_point].subtype == 0)));
        }
        current_point--;
    }
    return ++current_point;
}

int16_t find_corresponding_while(command_t *cmds, int16_t current_point) {
    int16_t opened = -1;
    current_point--;
    while (opened <= -1 && current_point >= 0) {
        if (cmds[current_point].type == 5 && cmds[current_point].subtype != while_end_byte[1]) {
            opened += ((int16_t) (1 - 2*(cmds[current_point].subtype == 0)));
        }
        current_point--;
    }
    return ++current_point;
}

int16_t get_next_point(int16_t current_point, int16_t max_point) {
    int16_t next_point = current_point + 1;
    return next_point == max_point ? -1 : next_point;
}

void print_command_struct(command_t *cmd) {
    printf("type %d\n", cmd->type);
    printf("stype %d\n", cmd->subtype);
    printf("var1 %d\n", cmd->var1);
    printf("var2 %d\n", cmd->var2);
    printf("var3 %d\n", cmd->var3);
    printf("var4 %d\n", cmd->var4);
    printf("failure %d\n", cmd->next_failure);
    printf("success %d\n", cmd->next_success);
    printf("data-type %d\n", cmd->type == 1? cmd->data_type : -1);
    //printf("%d %d %d %d %d %d %d %d %d\n", cmd->type, cmd->subtype, cmd->var1, cmd->var2, cmd->var3, cmd->var4, \
           cmd->next_success, cmd->next_failure, cmd->type == 1? cmd->data_type : -1);
}


int get_byte_code(command_t *cmds, int16_t line_no, char *code, var_ct extra[]) {
    int branch = 0;
    int loop = 0;
    int16_t pointer = 0;
    int16_t next_pointer = 0;
    char *line;
    char* code_pointer = code;
    int i; // error count

    DEBUG_PRINT(("total lines %d\n", line_no));
    while (pointer < line_no) {
        line = strtok_r(code_pointer, "\n", &code_pointer);

        DEBUG_PRINT(("\nLexer: Retrieved line of length %d %d:", branch, pointer));
        DEBUG_PRINT(("%s\n", line));
        //DEBUG_PRINT(("%s\n", code_pointer));

        char *token = strtok(line, " \t");

        // We can't use switch statements here.
        if (strcmp(token, return_ok_command) == 0) {
            set_command(&cmds[pointer], return_ok_byte, -1, -1);
        }

        else if (strcmp(token, return_error_command) == 0) {
            set_command(&cmds[pointer], return_error_byte, -1, -1);
        }
            // This else will be only found after if=1 branch
        else if (strcmp(token, else_command) == 0) {
            if (branch <= 0) {
                DEBUG_PRINT(("Syntax error: going to else without if command %d\n", branch));
                return CONTRACT_ADDITIONAL_ELSE_ERR;
            }

            next_pointer = find_corresponding_if(cmds, pointer);
            DEBUG_PRINT(("Found else after running if %d %d\n", branch, next_pointer));
            if (next_pointer == 0) {
                DEBUG_PRINT(("Syntax error: if without end-if %d %d\n", branch, next_pointer));
                return CONTRACT_LESS_END_IF_ERR;
            }
            set_command(&cmds[pointer], else_byte, get_next_point(pointer, line_no), -1);
            cmds[next_pointer].next_failure = pointer + 1;
            DEBUG_PRINT(("Changed next_failure of if %d to %d\n", next_pointer, pointer));
        }
            // This else will be only found after else=1 branch
        else if (strcmp(token, if_end_command) == 0) {
            if (branch <= 0) {
                DEBUG_PRINT(("Syntax error: going to exist without if command %d\n", branch));
                return CONTRACT_MORE_END_IF_ERR;
            }
            next_pointer = find_corresponding_if(cmds, pointer);
            DEBUG_PRINT(("\nFound end_if %d %d\n", branch, next_pointer));
            if (next_pointer == 0) {
                DEBUG_PRINT(("Syntax error: if without end-if %d\n", branch));
                return CONTRACT_LESS_END_IF_ERR;
            }
            set_command(&cmds[pointer], if_end_byte,  get_next_point(pointer, line_no), -1);
            // There is no else command.
            if (cmds[next_pointer].next_failure == -1) {
                cmds[next_pointer].next_failure = pointer;
                DEBUG_PRINT(("Changed next_failure of if %d to %d\n", next_pointer, pointer));
            }
            // There is an else command.
            else {
                cmds[cmds[next_pointer].next_failure - 1].next_failure = pointer;
                DEBUG_PRINT(("Changed next_failure of else %d to %d\n", cmds[next_pointer].next_failure - 1, pointer));
            }
            branch--;
        }

        else if (strcmp(token, while_end_command) == 0) {
            if (loop <= 0) {
                DEBUG_PRINT(("Syntax error: going to while_end without a while command %d\n", loop));
                return CONTRACT_WHILE_END_ERR;
            }
            next_pointer = find_corresponding_while(cmds, pointer);
            DEBUG_PRINT(("\nFound end_while %d %d\n", loop, next_pointer));
            if (next_pointer == 0) {
                DEBUG_PRINT(("Syntax error: going to while_end without a while command %d\n", loop));
                return CONTRACT_OPEN_LOOPS;
            }
            // Go back to the corresponding while command
            set_command(&cmds[pointer], while_end_byte,  next_pointer, -1);
            cmds[next_pointer].next_failure = get_next_point(pointer, line_no);
            DEBUG_PRINT(("Changed next_failure of while %d to %d\n", next_pointer, get_next_point(pointer, line_no)));
            loop--;
        }

        else if (strcmp(token, set_int_command) == 0) {
            set_command(&cmds[pointer], set_int_byte, get_next_point(pointer, line_no), -1);
            pre_set_int_gen(&cmds[pointer], token);
        }

        else if (strcmp(token, set_cint_command) == 0) {
            set_command(&cmds[pointer], set_cint_byte, get_next_point(pointer, line_no), -1);
            pre_set_cint_gen(&cmds[pointer], token, extra);
        }

        else if (strcmp(token, set_cint_from_in_command) == 0) {
            set_command(&cmds[pointer], set_cint_from_in_byte, get_next_point(pointer, line_no), -1);
            token = strtok(nullptr, " \t");
            cmds[pointer].var1 = (int16_t)  get_variable(token);
            token = strtok(nullptr, " \t\n");
            cmds[pointer].var2 = (int16_t) get_in_id(token);
            cmds[pointer].var3 = -1;
            cmds[pointer].var4 = -1;
        }

        else if (strcmp(token, set_cint_from_out_command) == 0) {
            set_command(&cmds[pointer], set_cint_from_out_byte, get_next_point(pointer, line_no), -1);
            token = strtok(nullptr, " \t");
            cmds[pointer].var1 = (int16_t)  get_variable(token);
            token = strtok(nullptr, " \t\n");
            cmds[pointer].var2 = (int16_t) get_out_id(token);
            cmds[pointer].var3 = -1;
            cmds[pointer].var4 = -1;
        }

        else if (strcmp(token, set_cint_from_in_pk_command) == 0) {
            set_command(&cmds[pointer], set_cint_from_in_pk_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, set_cint_from_out_pk_command) == 0) {
            set_command(&cmds[pointer], set_cint_from_out_pk_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, set_pk_command) == 0) {
            set_command(&cmds[pointer], set_pk_byte, get_next_point(pointer, line_no), -1);
            pre_set_pk_gen(&cmds[pointer], token, extra);
        }

        else if (strcmp(token, set_pk_from_in_command) == 0) {
            set_command(&cmds[pointer], set_pk_from_in_byte, get_next_point(pointer, line_no), -1);
            token = strtok(nullptr, " \t");
            cmds[pointer].var1 = (int16_t) get_variable(token);
            token = strtok(nullptr, " \t\n");
            cmds[pointer].var2 = (int16_t) get_in_id(token);
            cmds[pointer].var3 = -1;
            cmds[pointer].var4 = -1;
        }

        else if (strcmp(token, set_pk_from_out_command) == 0) {
            set_command(&cmds[pointer], set_pk_from_out_byte, get_next_point(pointer, line_no), -1);
            token = strtok(nullptr, " \t");
            cmds[pointer].var1 = (int16_t) get_variable(token);
            token = strtok(nullptr, " \t\n");
            cmds[pointer].var2 = (int16_t) get_out_id(token);
            cmds[pointer].var3 = -1;
            cmds[pointer].var4 = -1;
        }

        else if (strcmp(token, set_int_from_inlen_command) == 0) {
            set_command(&cmds[pointer], set_int_from_inlen_byte, get_next_point(pointer, line_no), -1);
            token = strtok(nullptr, " \t\n");
            cmds[pointer].var1 = (int16_t) get_variable(token);
            cmds[pointer].var2 = -1;
            cmds[pointer].var3 = -1;
            cmds[pointer].var4 = -1;
        }

        else if (strcmp(token, set_int_from_outlen_command) == 0) {
            set_command(&cmds[pointer], set_int_from_outlen_byte, get_next_point(pointer, line_no), -1);
            token = strtok(nullptr, " \t\n");
            cmds[pointer].var1 = (int16_t) get_variable(token);
            cmds[pointer].var2 = -1;
            cmds[pointer].var3 = -1;
            cmds[pointer].var4 = -1;
        }

        else if (strcmp(token, set_cint_from_int_command) == 0) {
            set_command(&cmds[pointer], set_cint_from_int_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, copy_int_command) == 0) {
            set_command(&cmds[pointer], copy_int_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, copy_cint_command) == 0) {
            set_command(&cmds[pointer], copy_cint_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }
            // -------------------
        else if (strcmp(token, add_int_command) == 0) {
            set_command(&cmds[pointer], add_int_byte, get_next_point(pointer, line_no), -1);
            set_3variables(&cmds[pointer], token);
        }

        else if (strcmp(token, add_cint_command) == 0) {
            set_command(&cmds[pointer], add_cint_byte, get_next_point(pointer, line_no), -1);
            set_3variables(&cmds[pointer], token);
        }

        else if (strcmp(token, sub_int_command) == 0) {
            set_command(&cmds[pointer], sub_int_byte, get_next_point(pointer, line_no), -1);
            set_3variables(&cmds[pointer], token);
        }

        else if (strcmp(token, sub_cint_command) == 0) {
            set_command(&cmds[pointer], sub_cint_byte, get_next_point(pointer, line_no), -1);
            set_3variables(&cmds[pointer], token);
        }

        else if (strcmp(token, mul_int_command) == 0) {
            set_command(&cmds[pointer], mul_int_byte, get_next_point(pointer, line_no), -1);
            set_3variables(&cmds[pointer], token);
        }

        else if (strcmp(token, mul_cint_command) == 0) {
            set_command(&cmds[pointer], mul_cint_byte, get_next_point(pointer, line_no), -1);
            set_3variables(&cmds[pointer], token);
        }

        else if (strcmp(token, div_int_command) == 0) {
            set_command(&cmds[pointer], div_int_byte, get_next_point(pointer, line_no), -1);
            set_3variables(&cmds[pointer], token);
        }
        else if (strcmp(token, divu_cint_command) == 0) {
            set_command(&cmds[pointer], divu_cint_byte, get_next_point(pointer, line_no), -1);
            set_4variables(&cmds[pointer], token);
        }

        else if (strcmp(token, div_cint_command) == 0) {
            set_command(&cmds[pointer], div_cint_byte, get_next_point(pointer, line_no), -1);
            set_4variables(&cmds[pointer], token);
        }

            // EQ - INT
        else if (strcmp(token, if_eq_int_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_eq_int_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // NEQ - INT
        else if (strcmp(token, if_neq_int_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_neq_int_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // LEQ - INT
        else if (strcmp(token, if_leq_int_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_leq_int_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // GEQ - INT
        else if (strcmp(token, if_geq_int_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_geq_int_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // EQ - CINT
        else if (strcmp(token, if_eq_cint_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_eq_cint_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // NEQ - CINT
        else if (strcmp(token, if_neq_cint_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_neq_cint_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // LEQ - CINT
        else if (strcmp(token, if_leq_cint_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_leq_cint_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // GEQ - CINT
        else if (strcmp(token, if_geq_cint_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_geq_cint_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // EQ - PK
        else if (strcmp(token, if_eq_pk_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_eq_pk_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

            // NEQ - PK
        else if (strcmp(token, if_neq_pk_command) == 0) {
            branch++;
            set_command(&cmds[pointer], if_neq_pk_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, while_int_eq_command) == 0) {
            loop++;
            set_command(&cmds[pointer], while_int_eq_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, while_int_neq_command) == 0) {
            loop++;
            set_command(&cmds[pointer], while_int_neq_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, while_int_leq_command) == 0) {
            loop++;
            set_command(&cmds[pointer], while_int_leq_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, while_int_geq_command) == 0) {
            loop++;
            set_command(&cmds[pointer], while_int_geq_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, while_cint_eq_command) == 0) {
            loop++;
            set_command(&cmds[pointer], while_cint_eq_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, while_cint_neq_command) == 0) {
            loop++;
            set_command(&cmds[pointer], while_cint_neq_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, while_cint_leq_command) == 0) {
            loop++;
            set_command(&cmds[pointer], while_cint_leq_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else if (strcmp(token, while_cint_geq_command) == 0) {
            loop++;
            set_command(&cmds[pointer], while_cint_geq_byte, get_next_point(pointer, line_no), -1);
            set_2variables(&cmds[pointer], token);
        }

        else {
            DEBUG_PRINT(("Syntax error %d %s\n", i++, token));
            FREE_LINE(line);
            return CONTRACT_SYNTAX_ERR;
        }
        pointer++;
    }
    if (branch != 0) {
        DEBUG_PRINT(("Syntax error 3: open branches %d\n", branch));
        return CONTRACT_OPEN_BRANCHES;
    }

    if (loop != 0) {
        DEBUG_PRINT(("Syntax error 3: open loops %d\n", loop));
        return CONTRACT_OPEN_LOOPS;
    }

    return 1;
}


