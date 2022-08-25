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

#ifndef CIP_SIS_LANG_UTILS
#define CIP_SIS_LANG_UTILS

void static store_size(FILE *zk_proof, int size) {
    fputc(size & 255, zk_proof);
    fputc((size >> 8) & 255, zk_proof);
    fputc((size >> 16) & 255, zk_proof);
    fputc((size >> 24) & 255, zk_proof);
}

int static read_size(FILE *zk_proof) {
    int size = 0;
    size = fgetc(zk_proof);
    size += fgetc(zk_proof) << 8;
    size += fgetc(zk_proof) << 16;
    size += fgetc(zk_proof) << 24;
    return size;
}


int static get_variable(char *token) {
    uint8_t j = 0;
    if (strlen(token) == 4) {
        j = token[3] - 48;
    }
    else if (strlen(token) == 5) {
        j = (token[4] - 48);
        j += 10 * (token[3] - 48);
    }
    return j;
}

int static get_extravariable(char *token) {
    uint8_t j = 0;
    if (strlen(token) == 4) {
        j = token[3] - 48;
    }
    else if (strlen(token) == 5) {
        j = (token[4] - 48);
        j += 10 * (token[3] - 48);
    }
    return j;
}

int static get_in_id(char *token) {
    uint8_t j = 0;
    if (strlen(token) == 3) {
        j = token[2] - 48;
    }
    else if (strlen(token) == 4) {
        j = (token[3] - 48);
        j += 10 * (token[2] - 48);
    }
    return j;
}

int static get_out_id(char *token) {
    uint8_t j = 0;
    if (strlen(token) == 4) {
        j = token[3] - 48;
    }
    else if (strlen(token) == 5) {
        j = (token[4] - 48);
        j += 10 * (token[3] - 48);
    }
    return j;
}

void static var_ct_copy(in_ct *out, in_ct *in) {
    out->v = in->v;
    poly_n_set(&out->u, &in->u);
    for (int i = 0; i < mk; i++) {
        poly_set(&out->k.k[i], &in->k.k[i]);
    }
    out->is_set_pk = in->is_set_pk;
    if (in->is_set_pk == 1)
        memcpy(out->pk, in->pk, PKID);
}

void static var_et_copy(in_et *out, in_et *in) {
    poly_n_set(&out->u, &in->u);
    out->is_set_pk = in->is_set_pk;
    if (in->is_set_pk == 1)
        memcpy(out->pk, in->pk, PKID);
}

void static var_pk_copy(var_ct *out, in_ct *in) {
    out->data.is_set_pk = in->is_set_pk;
    if (in->is_set_pk == 1)
        memcpy(out->data.pk, in->pk, PKID);
}

void static var_pk_ct_copy(var_ct *out, in_ct *in) {
    out->data.is_set_pk = in->is_set_pk;
    if (in->is_set_pk == 1)
        memcpy(out->data.pk, in->pk, PKID);
}

void static var_pk_et_copy(var_et *out, in_et *in) {
    out->data.is_set_pk = in->is_set_pk;
    if (in->is_set_pk == 1)
        memcpy(out->data.pk, in->pk, PKID);
}


#define FREE_LINE(line) do { \
    if (line != nullptr) { \
        free(line);          \
        line = nullptr; \
    }                        \
} while (0)

#endif //CIP_SIS_LANG_UTIL