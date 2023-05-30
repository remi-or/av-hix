#include "core_packages.c"

__m256i _mm256_sidesqr_epi8 (__m256i x, int left) {
    /* Given a vector x = [a, b, c, d, ...] of 32*int8 returns the vector 
    y = [a+b, c+d, ...] of 16*int16. */
    
    int masking_int;
    if (left) { masking_int = +1; }
    else      { masking_int = 1<<15; }
    const __m256i masked_x = _mm256_sign_epi8(x, _mm256_set1_epi16(masking_int));
    const __m256i masked_x2 = _mm256_maddubs_epi16(_mm256_set1_epi8(1), masked_x);
    return _mm256_mullo_epi16(masked_x2, masked_x2);
}

__m256i _mm256_chadd_epi8 (__m256i x) {
    /* Given a vector x = [a, b, c, d, ...] of 32*int8 returns the vector 
    y = [a+b, c+d, ...] of 16*int16. */
    
    return _mm256_maddubs_epi16(_mm256_set1_epi8(1), x);
}

__m256i _mm256_chmul_epi8(__m256i x) { 
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

__m256i _mm256_sumcubes_epi8 (__m256i x) {
    /* Given a vector x = [a, b, c, d, ...] of 32*int8 returns the vector 
    y = [a^3 + b^3 + c^3 + d^3, ...] of 8*int32. This functions uses the fact
    that (a+b)(a²+b²-ab) = a^3 + b^3*/
    
    // Prepare result accumulator
    __m256i result_acc = _mm256_set1_epi32(0);
    // Compute left-hand side [a+b, ...]
    const __m256i left_hand_side = _mm256_chadd_epi8(x);
    // Add first contribution (a+b)a²
    result_acc = _mm256_add_epi32(result_acc,
        _mm256_madd_epi16(left_hand_side, _mm256_sidesqr_epi8(x, 1)));
    // Add second contribution (a+b)b²
    result_acc = _mm256_add_epi32(result_acc,
        _mm256_madd_epi16(left_hand_side, _mm256_sidesqr_epi8(x, 0)));
    // Remove second contribution
    result_acc = _mm256_sub_epi32(result_acc, 
        _mm256_madd_epi16(left_hand_side, _mm256_chmul_epi8(x)));
    // _mm256_print_epi32(&result_acc);
    return result_acc;
}