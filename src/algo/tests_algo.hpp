//
// Created by Jayamine Alupotha
//

#ifndef CIP_SIS_TESTS_ALGO_H
#define CIP_SIS_TESTS_ALGO_H

void test_get_key_tau(mask_t *k) {
    uint8_t seed[r_BYTES];
    for (int i = 0; i < mk; ++i) get_mask_tau(&k->k[i], seed);
}

void test_get_key_tau1(mask_t *k) {
    uint8_t seed[r1_BYTES];
    for (int i = 0; i < mk; ++i) get_mask_tau1(&k->k[i], seed);
}

void test_get_key_tau2(mask_t *k) {
    uint8_t seed[r_BYTES];
    for (int i = 0; i < mk; ++i) get_mask_tau2(&k->k[i], seed);
}

void test_add_sub_d1() {
    int i;
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    uint8_t k_seed[mk][r_BYTES];

    zint_t v0[1];
    zint_t v1[1] = {1000};
    zint_t v2[1] = {2000};

    mask_t k0;
    mask_t k1;
    mask_t k2;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    com_t u;
    com_t u0;
    com_t u1;
    com_t u2;

    commit_from_int(&ctx, 1, &u1, k1.k, v1);
    commit_from_int(&ctx, 1, &u2, k2.k, v2);
    add_gen(&ctx, 1, &u0, v0, &k0, &u1, v1, &k1, &u2, v2, &k2);
    CHECK(add_ver(&ctx, &u0, &u1, &u2) == 0);

    commit_from_int(&ctx, 1, &u, k0.k, v0);
    CHECK(v0[0] == 3000 );
    CHECK(poly_n_compare(&u, &u0) == 0);

    sub_gen(&ctx, 1, &u0, v0, &k0, &u2, v2, &k2, &u1, v1, &k1);
    CHECK(v0[0] == 1000);
    CHECK(sub_ver(&ctx, &u0, &u2, &u1) == 0);

    commit_from_int(&ctx, 1, &u, k0.k, v0);
    CHECK(poly_n_compare(&u, &u0) == 0);

}

void test_add_sub_d2() {
    int i;
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    uint8_t k_seed[mk][r_BYTES];

    zint_t v0[2];
    zint_t v1[2] = {1000, 1000};
    zint_t v2[2] = {2000, 4000};

    mask_t k0;
    mask_t k1;
    mask_t k2;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    com_t u;
    com_t u0;
    com_t u1;
    com_t u2;

    commit_from_int(&ctx, 2, &u1, k1.k, v1);
    commit_from_int(&ctx, 2, &u2, k2.k, v2);
    add_gen(&ctx, 2, &u0, v0, &k0, &u1, v1, &k1, &u2, v2, &k2);
    CHECK(add_ver(&ctx, &u0, &u1, &u2) == 0);

    commit_from_int(&ctx, 2, &u, k0.k, v0);
    CHECK(v0[0] == 3000 && v0[1] == 5000);
    CHECK(poly_n_compare(&u, &u0) == 0);

    sub_gen(&ctx, 2, &u0, v0, &k0, &u2, v2, &k2, &u1, v1, &k1);
    CHECK(v0[0] == 1000 && v0[1] == 3000);
    CHECK(sub_ver(&ctx, &u0, &u2, &u1) == 0);

    commit_from_int(&ctx, 2, &u, k0.k, v0);
    CHECK(poly_n_compare(&u, &u0) == 0);

}

void test_mul_d1() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    zint_t v0[1];
    zint_t v1[1] = {1000};
    zint_t v2[1] = {2000};

    mask_t k0;
    mask_t k1;
    mask_t k2;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    uint8_t L1 = 12;

    com_t u0;
    com_t u1;
    com_t u2;

    commit_from_int(&ctx, 1, &u1, k1.k, v1);
    commit_from_int(&ctx, 1, &u2, k2.k, v2);

    mul_pt proof;
    mul_pt proof1;
    allocate_mul_proof(&proof, L1, 1);
    allocate_mul_proof(&proof1, L1, 1);

    mul_gen(&ctx, L1, 1, &proof,
            &u0, v0, &k0,
            &u1, v1, &k1, &u2, v2, &k2);

    CHECK(mul_ver(&ctx, &u0, &u1, &u2, &proof) == 0);

    uint8_t bytes[get_mul_pt_size(&proof)];
    mul_pt_to_bytes(bytes, &proof);
    bytes_to_mul_pt(&proof1, bytes);
    CHECK(mul_ver(&ctx, &u0, &u1, &u2, &proof1) == 0);

    commit_from_int(&ctx, 1, &u0, k0.k, v1);
    CHECK(mul_ver(&ctx, &u0, &u1, &u2, &proof) == -1);

    commit_from_int(&ctx, 1, &u0, k0.k, v0);
    CHECK(mul_ver(&ctx, &u0, &u1, &u2, &proof) == 0);

    deallocate_mul_proof(&proof);
    deallocate_mul_proof(&proof1);

}

void test_mul_d2() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    zint_t v0[2];
    zint_t v1[2] = {1000, 1000};
    zint_t v2[2] = {2000, 4000};

    mask_t k0;
    mask_t k1;
    mask_t k2;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    com_t u0;
    com_t u1;
    com_t u2;

    uint8_t L1 = 12;

    commit_from_int(&ctx, 2, &u1, k1.k, v1);
    commit_from_int(&ctx, 2, &u2, k2.k, v2);

    mul_pt proof;
    allocate_mul_proof(&proof, L1, 2);

    mul_gen(&ctx, L1, 2, &proof,
            &u0, v0, &k0,
            &u1, v1, &k1, &u2, v2, &k2);
    CHECK(mul_ver(&ctx, &u0, &u1, &u2, &proof) == 0);

    commit_from_int(&ctx, 2, &u0, k0.k, v1);
    CHECK(mul_ver(&ctx, &u0, &u1, &u2, &proof) == -1);

    commit_from_int(&ctx, 2, &u0, k0.k, v0);
    CHECK(mul_ver(&ctx, &u0, &u1, &u2, &proof) == 0);

    deallocate_mul_proof(&proof);

}

void test_equal_nequal_d1() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    poly v1[2];
    poly v2[2];
    poly_set_zero(&v1[0], 0, N);
    poly_set_zero(&v1[1], 0, N);
    poly_set_zero(&v2[0], 0, N);
    poly_set_zero(&v2[1], 0, N);
    v1[0].coef[0] = 100;
    v1[1].coef[0] = 200;
    v2[0].coef[0] = 100;
    v2[1].coef[0] = 200;

    mask_t k1;
    mask_t k2;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    com_t u1;
    com_t u2;

    commit(&ctx, &u1, k1.k, v1, nullptr);
    commit(&ctx, &u2, k2.k, v2, nullptr);

    eq_pt proof;
    CHECK(equal_gen(&ctx, &proof, &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(equal_ver(&ctx, &u1, &u2, &proof) == 0);

    uint8_t bytes[EQ_PT_BYTES];
    eq_pt_to_bytes(bytes, &proof);
    bytes_to_eq_pt(&proof, bytes);
    CHECK(equal_ver(&ctx, &u1, &u2, &proof) == 0);

    v1[0].coef[0]++;
    CHECK(equal_gen(&ctx, &proof, &u1, v1, &k1, &u2, v2, &k2) == -1);
    commit(&ctx, &u1, k1.k, v1, nullptr);
    CHECK(equal_ver(&ctx, &u1, &u2, &proof) == -1);

    neq_pt proof1;
    neq_pt proof11;
    allocate_neq_proof(&proof1, 8, 1);
    allocate_neq_proof(&proof11, 8, 1);
    zint_t vs1[2] = {10, 10};
    zint_t vs2[2] = {11, 14};
    commit_from_int(&ctx, 1, &u1, k1.k, vs1);
    commit_from_int(&ctx, 1, &u2, k2.k, vs2);
    CHECK(nequal_gen(&ctx, 8, 1, &proof1, &u1, vs1, &k1, &u2, vs2, &k2) == 0);
    CHECK(nequal_ver(&ctx, &u1, &u2, &proof1) == 0);

    uint8_t bytes1[get_neq_pt_size(&proof1)];
    neq_pt_to_bytes(bytes1, &proof1);
    bytes_to_neq_pt(&proof11, bytes1);
    CHECK(nequal_ver(&ctx, &u1, &u2, &proof11) == 0);

    vs2[0] = 10;
    CHECK(nequal_gen(&ctx, 20, 1, &proof1, &u1, vs1, &k1, &u2, vs2, &k2) == -1);
    commit_from_int(&ctx, 1, &u2, k2.k, vs2);
    CHECK(nequal_ver(&ctx, &u1, &u2, &proof1) == -1);

    deallocate_neq_proof(&proof1);
    deallocate_neq_proof(&proof11);

    leq_proof_t proof2;
    allocate_leq(&proof2, 3, 1);
    zint_t vs11[2] = {1, 2};
    zint_t vs21[2] = {2, 3};
    commit_from_int(&ctx, 1, &u1, k1.k, vs11);
    commit_from_int(&ctx, 1, &u2, k2.k, vs21);
    CHECK(leq_gen(&ctx, 3, 1, &proof2, &u1, vs11, &k1, &u2, vs21, &k2) == 0);
    CHECK(leq_ver(&ctx, &u1, &u2, &proof2) == 0);
    vs11[0] = 4;
    commit_from_int(&ctx, 1, &u1, k1.k, vs11);
    CHECK(leq_gen(&ctx, 3, 1, &proof2, &u1, vs11, &k1, &u2, vs21, &k2) == -1);

    deallocate_leq(&proof2);

    geq_proof_t proof3;
    allocate_leq(&proof3, 3, 1);
    zint_t vs12[2] = {3, 5};
    zint_t vs22[2] = {2, 3};
    commit_from_int(&ctx, 1, &u1, k1.k, vs12);
    commit_from_int(&ctx, 1, &u2, k2.k, vs22);
    CHECK(geq_gen(&ctx, 3, 1, &proof3, &u1, vs12, &k1, &u2, vs22, &k2) == 0);
    CHECK(geq_ver(&ctx, &u1, &u2, &proof3) == 0);
    vs12[0] = 1;
    commit_from_int(&ctx, 1, &u1, k1.k, vs12);
    CHECK(geq_gen(&ctx, 3, 1, &proof3, &u1, vs12, &k1, &u2, vs21, &k2) == -1);

    deallocate_geq(&proof3);
}


void test_equal_nequal_d2() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    poly v1[2];
    poly v2[2];
    poly_set_zero(&v1[0], 0, N);
    poly_set_zero(&v1[1], 0, N);
    poly_set_zero(&v2[0], 0, N);
    poly_set_zero(&v2[1], 0, N);
    v1[0].coef[0] = 100;
    v1[1].coef[0] = 200;
    v2[0].coef[0] = 100;
    v2[1].coef[0] = 200;

    mask_t k1;
    mask_t k2;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    com_t u1;
    com_t u2;

    commit(&ctx, &u1, k1.k, v1, nullptr);
    commit(&ctx, &u2, k2.k, v2, nullptr);

    eq_pt proof;
    CHECK(equal_gen(&ctx, &proof, &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(equal_ver(&ctx, &u1, &u2, &proof) == 0);

    v1[0].coef[0]++;
    CHECK(equal_gen(&ctx, &proof, &u1, v1, &k1, &u2, v2, &k2) == -1);
    commit(&ctx, &u1, k1.k, v1, nullptr);
    CHECK(equal_ver(&ctx, &u1, &u2, &proof) == -1);

    neq_pt proof1;
    allocate_neq_proof(&proof1, 8, 2);
    zint_t vs1[2] = {10, 10};
    zint_t vs2[2] = {11, 14};
    commit_from_int(&ctx, 2, &u1, k1.k, vs1);
    commit_from_int(&ctx, 2, &u2, k2.k, vs2);
    CHECK(nequal_gen(&ctx, 8, 2, &proof1, &u1, vs1, &k1, &u2, vs2, &k2) == 0);
    CHECK(nequal_ver(&ctx, &u1, &u2, &proof1) == 0);

    vs2[1] = 10;
    CHECK(nequal_gen(&ctx, 20, 2, &proof1, &u1, vs1, &k1, &u2, vs2, &k2) == -1);
    commit_from_int(&ctx, 2, &u2, k2.k, vs2);
    CHECK(nequal_ver(&ctx, &u1, &u2, &proof1) == -1);

    deallocate_neq_proof(&proof1);

    leq_proof_t proof2;
    allocate_leq(&proof2, 3, 2);
    zint_t vs11[2] = {1, 2};
    zint_t vs21[2] = {2, 3};
    commit_from_int(&ctx, 2, &u1, k1.k, vs11);
    commit_from_int(&ctx, 2, &u2, k2.k, vs21);
    CHECK(leq_gen(&ctx, 3, 2, &proof2, &u1, vs11, &k1, &u2, vs21, &k2) == 0);
    CHECK(leq_ver(&ctx, &u1, &u2, &proof2) == 0);
    vs11[1] = 4;
    commit_from_int(&ctx, 2, &u1, k1.k, vs11);
    CHECK(leq_gen(&ctx, 3, 2, &proof2, &u1, vs11, &k1, &u2, vs21, &k2) == -1);

    deallocate_leq(&proof2);

    geq_proof_t proof3;
    allocate_leq(&proof3, 3, 2);
    zint_t vs12[2] = {3, 5};
    zint_t vs22[2] = {2, 3};
    commit_from_int(&ctx, 2, &u1, k1.k, vs12);
    commit_from_int(&ctx, 2, &u2, k2.k, vs22);
    CHECK(geq_gen(&ctx, 3, 2, &proof3, &u1, vs12, &k1, &u2, vs22, &k2) == 0);
    CHECK(geq_ver(&ctx, &u1, &u2, &proof3) == 0);
    vs12[1] = 2;
    commit_from_int(&ctx, 2, &u1, k1.k, vs12);
    CHECK(geq_gen(&ctx, 3, 2, &proof3, &u1, vs12, &k1, &u2, vs21, &k2) == -1);

    deallocate_geq(&proof3);
}


void test_div_d1() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    zint_t v0[2];
    zint_t v1[2] = {10, 10};
    zint_t v2[2] = {2, 5};
    zint_t v3[2];

    mask_t k0;
    mask_t k1;
    mask_t k2;
    mask_t k3;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    com_t u0;
    com_t u1;
    com_t u2;
    com_t u3;

    commit_from_int(&ctx, 1, &u1, k1.k, v1);
    commit_from_int(&ctx, 1, &u2, k2.k, v2);

    div_upt proof;
    div_upt proof11;
    allocate_div_unsigned_proof(&proof, 5, 1);
    allocate_div_unsigned_proof(&proof11, 5, 1);

    CHECK(div_unsigned_gen(&ctx, 5, 1, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(v0[0] == 5 && v3[0] == 0);
    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof) == 0);

    uint8_t bytes[get_div_upt_size(&proof)];
    div_upt_to_bytes(bytes, &proof);
    bytes_to_div_upt(&proof11, bytes);
    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof11) == 0);

    v1[0] = 11;
    commit_from_int(&ctx, 1, &u1, k1.k, v1);
    CHECK(div_unsigned_gen(&ctx, 5, 1, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(v0[0] == 5 && v3[0] == 1);
    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof) == 0);

    v1[0] = 4294967292; // 2**32 - 4
    v1[0] = 11;
    commit_from_int(&ctx, 1, &u1, k1.k, v1);
    commit_from_int(&ctx, 1, &u1, k1.k, v1);
    CHECK(div_unsigned_gen(&ctx, 5, 1, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(v0[0] == 5 && v3[0] == 1);
    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof) == 0);

    v1[0] = -11;
    CHECK(div_unsigned_gen(&ctx, 5, 1, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == -1);

    v2[0] = 0;
    CHECK(div_unsigned_gen(&ctx, 5, 1, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == -1);

    deallocate_div_unsigned_proof(&proof);
    deallocate_div_unsigned_proof(&proof11);

    div_pt proof1;
    div_pt proof21;
    allocate_div_signed_proof(&proof1, 5, 1);
    allocate_div_signed_proof(&proof21, 5, 1);

    v1[0] = 11;
    v2[0] = 5;
    commit_from_int(&ctx, 1, &u1, k1.k, v1);
    commit_from_int(&ctx, 1, &u2, k2.k, v2);
    CHECK(div_signed_gen(&ctx, 5, 1, &proof1,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(v0[0] == 2 && v3[0] == 1);
    CHECK(div_signed_ver(&ctx, &u0, &u1, &u2, &u3, &proof1) == 0);

    uint8_t bytes1[get_div_pt_size(&proof1)];
    div_pt_to_bytes(bytes1, &proof1);
    bytes_to_div_pt(&proof21, bytes1);
    CHECK(div_signed_ver(&ctx, &u0, &u1, &u2, &u3, &proof21) == 0);

    v1[0] = 11;
    v2[0] = -5;
    commit_from_int(&ctx, 1, &u1, k1.k, v1);
    commit_from_int(&ctx, 1, &u2, k2.k, v2);
    CHECK(div_signed_gen(&ctx, 5, 1, &proof1,
                         &u0, v0, &k0,
                         &u3, v3, &k3,
                         &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(v0[0] == -2 && v3[0] == 1);
    CHECK(div_signed_ver(&ctx, &u0, &u1, &u2, &u3, &proof1) == 0);

    v2[0] = 0;
    CHECK(div_signed_gen(&ctx, 5, 1, &proof1,
                         &u0, v0, &k0,
                         &u3, v3, &k3,
                         &u1, v1, &k1, &u2, v2, &k2) == -1);

    deallocate_div_signed_proof(&proof1);
    deallocate_div_signed_proof(&proof21);
}


void test_div_d2() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    zint_t v0[2];
    zint_t v1[2] = {10, 10};
    zint_t v2[2] = {2, 5};
    zint_t v3[2];

    mask_t k0;
    mask_t k1;
    mask_t k2;
    mask_t k3;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    com_t u0;
    com_t u1;
    com_t u2;
    com_t u3;

    commit_from_int(&ctx, 2, &u1, k1.k, v1);
    commit_from_int(&ctx, 2, &u2, k2.k, v2);

    div_upt proof;
    allocate_div_unsigned_proof(&proof, 5, 2);

    CHECK(div_unsigned_gen(&ctx, 5, 2, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(v0[0] == 5 && v0[1] == 2 && v3[0] == 0 && v3[1] == 0);
    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof) == 0);

    v1[1] = 11;
    commit_from_int(&ctx, 2, &u1, k1.k, v1);
    CHECK(div_unsigned_gen(&ctx, 5, 2, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(v0[0] == 5 && v0[1] == 2 && v3[0] == 0 && v3[1] == 1);
    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof) == 0);

    v1[1] = -11;
    commit_from_int(&ctx, 2, &u1, k1.k, v1);
    CHECK(div_unsigned_gen(&ctx, 5, 2, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == 0);
    CHECK(v0[0] == 5 && v0[1] == -2 && v3[0] == 0 && v3[1] == -1);
    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof) == 0);

    v2[1] = 0;
    commit_from_int(&ctx, 2, &u1, k1.k, v1);
    CHECK(div_unsigned_gen(&ctx, 5, 2, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == -1);

    // Can't have negative remainders that are smaller than v2
    v1[0] = -11;
    v2[0] = -5;
    commit_from_int(&ctx, 2, &u1, k1.k, v1);
    commit_from_int(&ctx, 2, &u2, k2.k, v2);
    CHECK(div_unsigned_gen(&ctx, 5, 2, &proof,
                           &u0, v0, &k0,
                           &u3, v3, &k3,
                           &u1, v1, &k1, &u2, v2, &k2) == -1);
    CHECK(v0[0] == 2 && v3[0] == -1);
    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof) == -1);

    deallocate_div_unsigned_proof(&proof);

}



void test_max_min() {
    uint8_t seed[SEED_BYTES];

    RAND_bytes(seed, SEED_BYTES);
    context_t ctx = init(seed);

    mask_t k1;
    mask_t k2;
    test_get_key_tau(&k1);
    test_get_key_tau(&k2);

    com_t u1;
    com_t u2;

    uint8_t d = 1;

#if L == 32
    uint8_t L1 = 32; // From the tests, this is the max
    zint_t vs1[1] = {(((zint_t)1) << 32) - 1};
    zint_t vs2[1] = {(((zint_t)1) << 32) - 2};
#elif L == 56
    uint8_t L1 = 56; // From the tests, this is the max
    zint_t vs1[1] = {(((zint_t)1) << 56) - 1};
    zint_t vs2[1] = {(((zint_t)1) << 56) - 2};
#else
    uint8_t L1 = 56; // From the tests, this is the max
    zint_t vs1[1] = {(((zint_t)1) << 56) - 1};
    zint_t vs2[1] = {(((zint_t)1) << 56) - 2};
#endif

    neq_pt proof1;
    allocate_neq_proof(&proof1, L1, d);
    commit_from_int(&ctx, d, &u1, k1.k, vs1);
    commit_from_int(&ctx, d, &u2, k2.k, vs2);
    CHECK(nequal_gen(&ctx, L1, d, &proof1, &u1, vs1, &k1, &u2, vs2, &k2) == 0);
    CHECK(nequal_ver(&ctx, &u1, &u2, &proof1) == 0);

    deallocate_neq_proof(&proof1);

    com_t u0;
    com_t u3;
    mask_t k0;
    mask_t k3;
    zint_t vs0[1];
    zint_t vs3[1];

#if L == 32
    L1 = 32;
    vs1[0] = (zint_t) ("4294967295"); // 2**32 - 2
    vs2[0] = (zint_t) ("2147483648"); // 2**31
#elif L == 56
    L1 = 56;
    vs1[0] = (zint_t) ("72057594037927935"); // 2**56 - 1
    vs2[0] = (zint_t) ("36028797018963968"); // 2**55
#endif

    commit_from_int(&ctx, d, &u1, k1.k, vs1);
    commit_from_int(&ctx, d, &u2, k2.k, vs2);

    div_upt proof;
    allocate_div_unsigned_proof(&proof, L1, d);

    CHECK(div_unsigned_gen(&ctx, L1, d, &proof,
                           &u0, vs0, &k0,
                           &u3, vs3, &k3,
                           &u1, vs1, &k1, &u2, vs2, &k2) == 0);
#if L == 32
    CHECK(vs0[0] == 1 && vs3[0] == 2147483647);
#elif L == 56
    CHECK(vs0[0] == 1 && vs3[0] == (zint_t)("36028797018963967"));
#endif

    CHECK(div_unsigned_ver(&ctx, &u0, &u1, &u2, &u3, &proof) == 0);
    deallocate_div_unsigned_proof(&proof);

#if L == 32
    cout << L1 << "\n";
    vs3[0] = (zint_t) ("4294967295"); // 2**32 - 1
#elif L == 56
    cout << L1 << "\n";
    vs3[0] = (zint_t) ("72057594037927935"); // 2**56 - 1
#endif

    rp_t rp;
    commit_from_int(&ctx, d, &u3, k3.k, vs3);
    allocate_rp(&rp, L1, d);
    range_gen(&ctx, L1, d, &rp, &u3, vs3, k3.k);
    CHECK(range_ver(&ctx, &u3, &rp) == 0);
    deallocate_rp(&rp);
}

#endif //CIP_SIS_TESTS_ALGO_H
