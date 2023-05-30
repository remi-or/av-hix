#include "../test_functions.c"


static __m256i _mm256_chmul_epi8_dev(__m256i x) { 
    /* Given a vector x = [a, b, c, d, ...] of 32*int8 returns the vector 
    y = [ab, cd, ...] of 16*int16. */

    // Offset the elements equal to -128 by 1 to get -127
    const __m256i equal_to_minus128 = _mm256_abs_epi8(
        _mm256_cmpeq_epi8(x, _mm256_set1_epi8(-128)));
    // TODO : avoid this with sub + time result
    const __m256i offseted = _mm256_add_epi8(x, equal_to_minus128);
    // Switch elements of x to get [b, a, d, c, ...]
    const __m256i permutation = _mm256_set_epi8(
        30, 31, 28, 29, 26, 27, 24, 25, 22, 23, 20, 21, 18, 19, 16, 17,
        14, 15, 12, 13, 10, 11,  8,  9,  6,  7,  4,  5,  2,  3,  0,  1);
    const __m256i switched = _mm256_shuffle_epi8(offseted, permutation);
    // Compute [ab, cd, ...] + [offset error]
    const __m256i approx_doubled_result = _mm256_maddubs_epi16(
        _mm256_abs_epi8(switched),  _mm256_sign_epi8(offseted, switched));
    const __m256i approx_result = _mm256_srai_epi16(approx_doubled_result, 1);
    // Compute offset error type 1 (only 1 of the elements in the pair was -128)
    const __m256i offset_error_1 = 
        _mm256_maddubs_epi16(equal_to_minus128, switched);
    // Compute offset error type 2 (both of the elements in the pair were -128)
    const __m256i offset_error_2 = 
        _mm256_cmpeq_epi16(equal_to_minus128, _mm256_set1_epi16(257));
    // Correct all offset errors
    const __m256i result = _mm256_sub_epi16(
        _mm256_sub_epi16(approx_result, offset_error_1), offset_error_2);
    return result;
}

int _mm256_chmul_epi8_ref(int a, int b) {
    return a*b;
}


#define VECTOR_FN _mm256_chmul_epi8_dev
#define SCALAR_FN _mm256_chmul_epi8_ref

int main () {
    int a, b, ref;
    __m256i x, y;
    // Loop over all 2*int8 configurations
    for (size_t iter = 0; iter < (1<<23); iter += 1) {
        // Prepare input
        if (iter < (1<<16)) { x = _mm256_set_epi16(iter-(1<<15), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); } 
        else                { _mm256_rand_epi8(&x); }
        // Compute predicted result
        y = VECTOR_FN(x);
        // Check it against reference function
        int8_t* ptr_x = (int8_t*) &x;
        int16_t* ptr_y = (int16_t*) &y;
        // Iterate over predicted results (32 int8 / 2 perpair = 16 pairs)
        for (int j = 0; j < 16; j++ ) {
            a = (int) ptr_x[2*j];
            b = (int) ptr_x[2*j+1];
            ref = SCALAR_FN(a, b);
            if (ptr_y[j] != ref) {
                printf("With inputs: %d, %d\n", a, b);
                printf("The predicted result was: %d\n", ptr_y[j]);
                printf("When the reference was: %d\n\n", ref);
                _mm256_print_epi8(&x);
                _mm256_print_epi16(&y);
                return 1;
            } 
        }
    }
    return 0;
}