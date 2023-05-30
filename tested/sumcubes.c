#include "../test_functions.c"
#include "../avehix.c"


static __m256i _mm256_sumcubes_epi8_dev (__m256i x) {
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

static int _mm256_sumcubes_epi8_ref (int a, int b, int c, int d) {
    int acc = 0;
    acc += a*a*a;
    acc += b*b*b;
    acc += c*c*c;
    acc += d*d*d;
    return acc;
}

#define VECTOR_FN _mm256_sumcubes_epi8_dev
#define SCALAR_FN _mm256_sumcubes_epi8_ref

int main () {
    int a, b, c, d, ref;
    __m256i x, y;
    // Loop over all 2*int8 configurations then random
    for (size_t iter = 0; iter < (1<<20); iter += 1) {
        // Prepare input
        if (iter < (1<<16)) { x = _mm256_set_epi16(iter-(1<<15), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, -128); } 
        else                { _mm256_rand_epi8(&x); }
        // Compute predicted result
        y = VECTOR_FN(x);
        // Check it against reference function
        int8_t* ptr_x = (int8_t*) &x;
        int32_t* ptr_y = (int32_t*) &y;
        // Iterate over predicted results (32 int8 / 4 per sample = 8 samples)
        for (int j = 0; j < 8; j++ ) {
            a = (int) ptr_x[4*j];
            b = (int) ptr_x[4*j+1];
            c = (int) ptr_x[4*j+2];
            d = (int) ptr_x[4*j+3];
            ref = SCALAR_FN(a, b, c, d);
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