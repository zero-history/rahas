//
// Created by Jayamine Alupotha
//

#ifndef CIP_SIS_TESTS_INPUTS_H
#define CIP_SIS_TESTS_INPUTS_H

// when mv = 1
void test_inputs_d1() {
    int i;
    poly_n u;
    poly_n u1;
    poly k[mk];
    poly v[mv];
    uint8_t seed[SEED_BYTES];
    uint8_t k_seed[mk][r_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    uint8_t d = 1;

    // Generate short vector
    for (i = 0; i < mk; ++i) get_mask_tau(&k[i], k_seed[i]);
    for (i = 0; i < mv; ++i) poly_set_zero(&v[i], 0, N);

    zint_t vs[1] = {100};
    v[0].coef[0] = vs[0];

    commit(&ctx, &u, k, v, nullptr);
    CHECK(open(&ctx, &u, v, k) == 0);

    commit_from_int(&ctx, d, &u1, k, vs);
    CHECK(open(&ctx, &u1, v, k) == 0);

    CHECK(poly_n_compare(&u, &u1) == 0);

    k[0].coef[0] = k[0].coef[0] + 1;
    CHECK(open(&ctx, &u, v, k) == -1);
    k[0].coef[0] = k[0].coef[0] - 1;

    v[0].coef[0] = GAMMA1 + 1;
    commit(&ctx, &u, k, v, nullptr);
    CHECK(open(&ctx, &u, v, k) == -1);

    v[0].coef[0] = 100;

    commit(&ctx, &u, k, v, nullptr);
    CHECK(open(&ctx, &u, v, k) == 0);

    rp_t rp;
    CHECK(allocate_rp(&rp, 10, 5) == -1);
    CHECK(allocate_rp(&rp, 10, d) == 0);
    range_gen(&ctx, 10, d, &rp, &u, vs, k);
    CHECK(range_ver(&ctx, &u, &rp) == 0);


    vs[0] = 1000000;
    CHECK(rcommit(&ctx, 10, d, &rp, &u1, vs, k) == -1);
    /*vs[0] = -100;
    CHECK(rcommit(&ctx, 10, d, &rp, &u1, vs, k) == -1);*/
    vs[0] = 100;
    CHECK(rcommit(&ctx, 10, d, &rp, &u1, vs, k) == 0);

    CHECK(poly_n_compare(&u, &u1) == 0);

    /*vs[0] = 50;
    range_gen(&ctx, 10, d, &rp, &u, vs, k);
    CHECK(range_ver(&ctx, &u, &rp) == -1);*/

    /*vs[0] = 150;
    range_gen(&ctx, 10, d, &rp, &u, vs, k);
    CHECK(range_ver(&ctx, &u, &rp) == -1);*/

    vs[0] = 100;
    range_gen(&ctx, 10, d, &rp, &u, vs, k);
    CHECK(range_ver(&ctx, &u, &rp) == 0);

    uint8_t bytes[get_rp_size(&rp)];
    rp_to_bytes(bytes, &rp);

    rp_t rp1;
    CHECK(allocate_rp(&rp1, bytes[0], bytes[1]) == 0);
    bytes_to_rp(&rp1, bytes);
    int j;
    for (i = 0; i < rp.d; i++)
        for (j = 0; j < rp.L1; j++) {
            CHECK(poly_compare(&rp.z[i][j], &rp1.z[i][j]) == 0);
        }
    for (i = 0; i < mk; i++) {
        CHECK(poly_compare(&rp.s[i], &rp1.s[i]) == 0);
    }
    CHECK(poly_n_compare(&rp.t1, &rp1.t1) == 0);

    deallocate_rp(&rp);
    deallocate_rp(&rp1);
}

void test_inputs_d2() {
    int i;
    poly_n u;
    poly_n u1;
    poly k[mk];
    poly v[mv];
    uint8_t seed[SEED_BYTES];
    uint8_t k_seed[mk][r_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    // Generate short vector
    for (i = 0; i < mk; ++i) get_mask_tau(&k[i], k_seed[i]);
    for (i = 0; i < mv; ++i) poly_set_zero(&v[i], 0, N);

    zint_t vs[2] = {100, 200};
    v[0].coef[0] = vs[0];
    v[1].coef[0] = vs[1];

    commit(&ctx, &u, k, v, nullptr);
    CHECK(open(&ctx, &u, v, k) == 0);

    commit_from_int(&ctx, 2, &u1, k, vs);
    CHECK(open(&ctx, &u1, v, k) == 0);

    CHECK(poly_n_compare(&u, &u1) == 0);

    k[0].coef[0] = k[0].coef[0] + 1;
    CHECK(open(&ctx, &u, v, k) == -1);
    k[0].coef[0] = k[0].coef[0] - 1;

    v[0].coef[0] = GAMMA1 + 1;
    commit(&ctx, &u, k, v, nullptr);
    CHECK(open(&ctx, &u, v, k) == -1);

    v[0].coef[0] = 100;
    v[1].coef[0] = 200;

    commit(&ctx, &u, k, v, nullptr);
    CHECK(open(&ctx, &u, v, k) == 0);

    rp_t rp;
    CHECK(allocate_rp(&rp, 10, 5) == -1);
    CHECK(allocate_rp(&rp, 10, 2) == 0);
    range_gen(&ctx, 10, 2, &rp, &u, vs, k);
    CHECK(range_ver(&ctx, &u, &rp) == 0);

    vs[0] = 1000000;
    CHECK(rcommit(&ctx, 10, 2, &rp, &u1, vs, k) == -1);
    vs[0] = -100;
    CHECK(rcommit(&ctx, 10, 2, &rp, &u1, vs, k) == -1);
    vs[0] = 100;
    CHECK(rcommit(&ctx, 10, 2, &rp, &u1, vs, k) == 0);

    CHECK(poly_n_compare(&u, &u1) == 0);

    vs[0] = 50;
    range_gen(&ctx, 10, 2, &rp, &u, vs, k);
    CHECK(range_ver(&ctx, &u, &rp) == -1);

    vs[1] = 150;
    range_gen(&ctx, 10, 2, &rp, &u, vs, k);
    CHECK(range_ver(&ctx, &u, &rp) == -1);

    vs[0] = 100;
    vs[1] = 200;
    range_gen(&ctx, 10, 2, &rp, &u, vs, k);
    CHECK(range_ver(&ctx, &u, &rp) == 0);

    deallocate_rp(&rp);
}

void test_inputs() {
    // We are going to change mv by changing "params.h"
#if mv == 1
    test_inputs_d1();
#else
    test_inputs_d2();
#endif
}

#endif //CIP_SIS_TESTS_INPUTS_H
