#include "../test_functions.c"

static __m256i _mm256_sidesqr_epi8_dev (__m256i x, int left) {
    /* Given a vector x = [a, b, c, d, ...] of 32*int8 returns the vector 
    y = [a+b, c+d, ...] of 16*int16. */
    
    int masking_int;
    if (left) { masking_int = +1; }
    else      { masking_int = 1<<15; }
    const __m256i masked_x = _mm256_sign_epi8(x, _mm256_set1_epi16(masking_int));
    const __m256i masked_x2 = _mm256_maddubs_epi16(_mm256_set1_epi8(1), masked_x);
    return _mm256_mullo_epi16(masked_x2, masked_x2);
}

int _mm256_sidesqr_epi8_ref(int a, int b, int left) {
    if (left) { return a*a; }
    else      { return b*b; }
}

#define VECTOR_FN(x, left) _mm256_sidesqr_epi8_dev(x, left)
#define SCALAR_FN(a, b, left) _mm256_sidesqr_epi8_ref(a, b, left)

int main () {
    int a, b, left, ref;
    __m256i x, y;
    // Loop over all 2*int8 configurations
    for (size_t iter = 0; iter < (1<<23); iter += 1) {
        // Prepare input
        if (iter < (1<<16)) { x = _mm256_set_epi16(iter-(1<<15), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); } 
        else                { _mm256_rand_epi8(&x); }
        left = rand() % 2;
        // Compute predicted result
        y = VECTOR_FN(x, left);
        // Check it against reference function
        int8_t* ptr_x = (int8_t*) &x;
        int16_t* ptr_y = (int16_t*) &y;
        // Iterate over predicted results (32 int8 / 2 perpair = 16 pairs)
        for (int j = 0; j < 16; j++ ) {
            a = (int) ptr_x[2*j];
            b = (int) ptr_x[2*j+1];
            ref = SCALAR_FN(a, b, left);
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
