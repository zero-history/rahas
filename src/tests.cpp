// CPP Program to demonstrate Big Integer data type

#include "rahas.cpp"


#include "tests_lattice.hpp"
#include "tests_inputs.hpp"
#include "tests_algo.hpp"
#include "lang/tests_lang.hpp"


int main() {

    clock_t start, end;
    float elapsed_time;

    start = clock();

    /*test_reduce64();
    test_get_masks();
    test_check_norm();
    test_ntt();
    test_pack();
    test_challenge_poly();

    test_matrix_mul();
    test_inputs();

#if mv == 1
    test_add_sub_d1();
    test_mul_d1();
    test_equal_nequal_d1();
    test_div_d1();
    printf("Going to max_min\n");

#else
    test_add_sub_d2();
    test_mul_d2();
    test_equal_nequal_d2();
    test_div_d2();
#endif

    //test_max_min();*/

    /*test_escrow();
    test_escrow2();
    test_lang_basic();*/
    test_for_basic();

    end = clock();
    elapsed_time = (float)(end - start) / (float)CLOCKS_PER_SEC;
    printf("Elapsed time: %f seconds\n", elapsed_time);

    cout << "lattice tests passed!\n";

    return 0;
}

