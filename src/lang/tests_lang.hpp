//
// Created by Jayamine Alupotha
//

#ifndef CIP_SIS_TESTS_LANG_H
#define CIP_SIS_TESTS_LANG_H

int var_ct_compare(var_ct *out, var_ct *in) {
    int val = 0;
    if (poly_n_compare(&out->data.u, &in->data.u) != 0)
        return -1;
    for (int i = 0; i < mk; i++) {
        if (poly_compare(&out->data.k.k[i], &in->data.k.k[i]))
            return -1;
    }
    if (in->data.is_set_pk != out->data.is_set_pk)
        return -1;
    if (in->data.is_set_pk == 1)
        if (memcmp(out->data.pk, in->data.pk, PKID) != 0)
            return -1;
    return 0;
}

void test_lang_basic() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    uint8_t key1[MASKKEY_BYTES];
    uint8_t key2[MASKKEY_BYTES];
    uint8_t key3[MASKKEY_BYTES];
    uint8_t key4[MASKKEY_BYTES];
    RAND_bytes(key1, MASKKEY_BYTES);
    RAND_bytes(key2, MASKKEY_BYTES);
    RAND_bytes(key3, MASKKEY_BYTES);
    RAND_bytes(key4, MASKKEY_BYTES);

    uint8_t L1 = 25;

    var_ct *extra_gen;
    extra_gen = (var_ct *) malloc(3 * sizeof(var_ct));

    // Set extra
    extra_gen[0].type = TYPE_CINT; // no public key
    extra_gen[1].type = TYPE_PK; // with public key

    // one percent
    in_ct_gen(&ctx, &extra_gen[0].data, 2, key1, 0, nullptr);
    extra_gen[1].data.is_set_pk = 1;
    memset(extra_gen[1].data.pk, 'a', PKID);

    char fp_path[] = "../test_samples/test_set.zksc";
    char zk_path[] = "../test_samples/test.zk";

    command_set cmds;

    CHECK(pre_parser(&cmds, fp_path, extra_gen) == 1);

    CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds, \
                           0, nullptr, 0, nullptr, 3, extra_gen) == 1);

    CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               0, nullptr, 0, nullptr) == 1);

    char fp_path2[] = "../test_samples/test_if.zksc";

    CHECK(pre_parser(&cmds, fp_path2, extra_gen) == 1);

    CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds, \
                           0, nullptr, 0, nullptr, 3, extra_gen) == 1);

    CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               0, nullptr, 0, nullptr) == 1);

    char fp_path_err_else1[] = "../test_samples/test_else_err_1.zksc";

    // If without end if
    free(cmds.set);
    CHECK(pre_parser(&cmds, fp_path_err_else1, extra_gen) == CONTRACT_OPEN_BRANCHES);

    //CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds, \
                           0, nullptr, 0, nullptr, 3, extra_gen) == CONTRACT_OPEN_BRANCHES);

    //CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               0, nullptr, 0, nullptr) == CONTRACT_OPEN_BRANCHES);


    char fp_path_err_else2[] = "../test_samples/test_else_err_2.zksc";

    // if and else without end-f
    CHECK(pre_parser(&cmds, fp_path_err_else2, extra_gen) == CONTRACT_OPEN_BRANCHES);

    //CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds, \
                           0, nullptr, 0, nullptr, 3, extra_gen) == CONTRACT_LESS_END_IF_ERR);

    //CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               0, nullptr, 0, nullptr) == CONTRACT_LESS_END_IF_ERR);

    char fp_path_err_else0[] = "../test_samples/test_else_err_0.zksc";

    // Additional else
    CHECK(pre_parser(&cmds, fp_path_err_else0, extra_gen) == CONTRACT_ADDITIONAL_ELSE_ERR);

    //CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds,\
                           0, nullptr, 0, nullptr, 3, extra_gen) == CONTRACT_ADDITIONAL_ELSE_ERR);

    //CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               0, nullptr, 0, nullptr) == CONTRACT_ADDITIONAL_ELSE_ERR);

    char fp_path_test_end_if_err_0[] = "../test_samples/test_end_if_err_0.zksc";

    // Additional end-if
    CHECK(pre_parser(&cmds, fp_path_test_end_if_err_0, extra_gen) == CONTRACT_MORE_END_IF_ERR);

    //CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds, \
                           0, nullptr, 0, nullptr, 3, extra_gen) == CONTRACT_MORE_END_IF_ERR);

    //CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               0, nullptr, 0, nullptr) == CONTRACT_MORE_END_IF_ERR);

    free(extra_gen);
}


void test_escrow() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    uint8_t key1[MASKKEY_BYTES];
    uint8_t key2[MASKKEY_BYTES];
    uint8_t key3[MASKKEY_BYTES];
    uint8_t key4[MASKKEY_BYTES];
    RAND_bytes(key1, MASKKEY_BYTES);
    RAND_bytes(key2, MASKKEY_BYTES);
    RAND_bytes(key3, MASKKEY_BYTES);
    RAND_bytes(key4, MASKKEY_BYTES);

    uint8_t L1 = 25;

    var_ct *extra_gen;
    extra_gen = (var_ct *) malloc(3 * sizeof(var_ct));

    // Set extra
    extra_gen[0].type = TYPE_CINT;
    extra_gen[1].type = TYPE_PK;
    extra_gen[2].type = TYPE_PK;

    // one percent
    in_ct_gen(&ctx, &extra_gen[0].data, 2, key1, 0, nullptr);
    // Alice's public key
    extra_gen[1].data.is_set_pk = 1;
    memset(extra_gen[1].data.pk, 'a', PKID);
    // Bob's public key
    extra_gen[2].data.is_set_pk = 1;
    memset(extra_gen[2].data.pk, 'b', PKID);

    // Set in
    coin_ct in[2];
    coin_ct out[2];
    coin_ct_init(&in[0], L1);
    coin_ct_init(&in[1], L1);
    coin_ct_init(&out[0], L1);
    coin_ct_init(&out[1], L1);

    coin_gen(&ctx, L1, &in[0], 10, key1);
    coin_gen(&ctx, L1, &in[1], 1000, key2);
    coin_gen(&ctx, L1, &out[0], 30, key3);
    coin_gen(&ctx, L1, &out[1], 980, key4);

    in[0].c.is_set_pk = 1;
    in[1].c.is_set_pk = 1;
    out[0].c.is_set_pk = 1;
    out[1].c.is_set_pk = 1;
    memset(in[0].c.pk, 'e', PKID);
    memset(in[1].c.pk, 'a', PKID);
    memset(out[0].c.pk, 'e', PKID);
    memset(out[1].c.pk, 'b', PKID);

    coin_et in1[2];
    coin_et out1[2];
    coin_et_init(&in1[0], L1);
    coin_et_init(&in1[1], L1);
    coin_et_init(&out1[0], L1);
    coin_et_init(&out1[1], L1);
    coin_ct_to_et(&in1[0], &in[0]);
    coin_ct_to_et(&in1[1], &in[1]);
    coin_ct_to_et(&out1[0], &out[0]);
    coin_ct_to_et(&out1[1], &out[1]);

    char fp_path[] = "../test_samples/tester_escrow.zksc";
    char zk_path[] = "../test_samples/tester_escrow.zk";

    // Send coins to Bob
    command_set cmds;
    CHECK(pre_parser(&cmds, fp_path, extra_gen) == 1);
    CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds,
                           2, in, 2, out, 20, extra_gen) == 1);
    CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               2, in1, 2, out1) == 1);

    // Send coins to Alice
    memset(out1[1].c.pk, 'a', PKID);
    CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               2, in1, 2, out1) == 1);

    free(cmds.set);
    free(extra_gen);
    coin_ct_free(&in[0]);
    coin_ct_free(&in[1]);
    coin_ct_free(&out[0]);
    coin_ct_free(&out[1]);
    coin_et_free(&in1[0]);
    coin_et_free(&in1[1]);
    coin_et_free(&out1[0]);
    coin_et_free(&out1[1]);
}


void test_escrow2() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    uint8_t key1[MASKKEY_BYTES];
    uint8_t key2[MASKKEY_BYTES];
    uint8_t key3[MASKKEY_BYTES];
    uint8_t key4[MASKKEY_BYTES];
    RAND_bytes(key1, MASKKEY_BYTES);
    RAND_bytes(key2, MASKKEY_BYTES);
    RAND_bytes(key3, MASKKEY_BYTES);
    RAND_bytes(key4, MASKKEY_BYTES);

    uint8_t L1 = 25;
    int percentage = 2;
    int s = 1000;

    var_ct *extra_gen;
    extra_gen = (var_ct *) malloc(4 * sizeof(var_ct));

    // Set extra
    extra_gen[0].type = TYPE_CINT;
    extra_gen[1].type = TYPE_PK;
    extra_gen[2].type = TYPE_PK;
    extra_gen[3].type = TYPE_PK;
    // one percent
    in_ct_gen(&ctx, &extra_gen[0].data, percentage, key1, 0, nullptr);
    // Alice's public key
    extra_gen[1].data.is_set_pk = 1;
    memset(extra_gen[1].data.pk, 'a', PKID);
    // Bob's public key
    extra_gen[2].data.is_set_pk = 1;
    memset(extra_gen[2].data.pk, 'b', PKID);
    // Escrow's public key
    extra_gen[3].data.is_set_pk = 1;
    memset(extra_gen[3].data.pk, 'e', PKID);


    // Set in
    coin_ct in[1];
    coin_ct out[2];
    coin_ct_init(&in[0], L1);
    coin_ct_init(&out[0], L1);
    coin_ct_init(&out[1], L1);
    coin_gen(&ctx, L1, &in[0], s, key2);
    coin_gen(&ctx, L1, &out[0], (s * percentage)/100, key3);
    coin_gen(&ctx, L1, &out[1], s - (s * percentage)/100, key4);
    in[0].c.is_set_pk = 1;
    out[0].c.is_set_pk = 1;
    out[1].c.is_set_pk = 1;
    memset(in[0].c.pk, 'e', PKID);
    memset(out[0].c.pk, 'e', PKID);
    memset(out[1].c.pk, 'b', PKID);

    coin_et in1[1];
    coin_et out1[2];
    coin_et_init(&in1[0], L1);
    coin_et_init(&out1[0], L1);
    coin_et_init(&out1[1], L1);
    coin_ct_to_et(&in1[0], &in[0]);
    coin_ct_to_et(&out1[0], &out[0]);
    coin_ct_to_et(&out1[1], &out[1]);

    char fp_path[] = "../test_samples/tester_escrow2.zksc";
    char zk_path[] = "../test_samples/tester_escrow2.zk";

    // Send coins to Bob
    command_set cmds;
    CHECK(pre_parser(&cmds, fp_path, extra_gen) == 1);
    CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds,
                           2, in, 2, out, 20, extra_gen) == 1);
    CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               2, in1, 2, out1) == 1);

    // Send coins to Alice
    memset(out1[1].c.pk, 'a', PKID);
    CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               2, in1, 2, out1) == 1);

    free(extra_gen);

    free(cmds.set);
    coin_ct_free(&in[0]);
    coin_ct_free(&out[0]);
    coin_ct_free(&out[1]);
    coin_et_free(&in1[0]);
    coin_et_free(&out1[0]);
    coin_et_free(&out1[1]);
}

void test_for_basic() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    uint8_t key1[MASKKEY_BYTES];
    uint8_t key2[MASKKEY_BYTES];
    uint8_t key3[MASKKEY_BYTES];
    uint8_t key4[MASKKEY_BYTES];
    RAND_bytes(key1, MASKKEY_BYTES);
    RAND_bytes(key2, MASKKEY_BYTES);
    RAND_bytes(key3, MASKKEY_BYTES);
    RAND_bytes(key4, MASKKEY_BYTES);

    uint8_t L1 = 25;

    var_ct *extra_gen;
    extra_gen = (var_ct *) malloc(4 * sizeof(var_ct));

    // Set extra
    extra_gen[0].type = TYPE_CINT;
    extra_gen[1].type = TYPE_CINT;
    extra_gen[2].type = TYPE_CINT;
    extra_gen[3].type = TYPE_CINT;

    in_ct_gen(&ctx, &extra_gen[0].data, 100, key1, 0, nullptr);
    in_ct_gen(&ctx, &extra_gen[1].data, 700, key2, 0, nullptr);
    in_ct_gen(&ctx, &extra_gen[2].data, 1000, key3, 0, nullptr);
    in_ct_gen(&ctx, &extra_gen[3].data, 600, key4, 0, nullptr);


    char fp_path[] = "../test_samples/test_while.zksc";
    char zk_path[] = "../test_samples/test.zk";

    // Send coins to Bob

    command_set cmds;
    CHECK(pre_parser(&cmds, fp_path, extra_gen) == 1);

    CHECK(parser_recursive(&ctx, L1,  zk_path, &cmds, 0, nullptr, 0, nullptr, 4, extra_gen) == 1);
    CHECK(parser_recursive_ver(&ctx, L1, zk_path, &cmds, \
                               0, nullptr, 0, nullptr) == 1);
    free(cmds.set);

    char fp_path_err_0[] = "../test_samples/test_while_err_0.zksc";
    CHECK(pre_parser(&cmds, fp_path_err_0, extra_gen) == CONTRACT_OPEN_LOOPS);

    char fp_path_err_1[] = "../test_samples/test_while_err_1.zksc";
    CHECK(pre_parser(&cmds, fp_path_err_1, extra_gen) == CONTRACT_WHILE_END_ERR);

    free(extra_gen);
}




#endif //CIP_SIS_TESTS_LANG_H
