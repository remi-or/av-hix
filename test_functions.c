#include "core_packages.c"

//---------------------------------------------------------------------------//
//                            AVX Print functions                            //
//---------------------------------------------------------------------------//

void _mm256_print_epi8(__m256i* src)  {
    int8_t* ptr = (int8_t*) src;
    for (int i = 0; i < 32; i++) { printf("%d ", ptr[i]); }
    printf("\n");
}
void _mm256_print_epu8(__m256i* src)  {
    uint8_t* ptr = (uint8_t*) src;
    for (int i = 0; i < 32; i++) { printf("%d ", ptr[i]); }
    printf("\n");
}
void _mm256_print_epi16(__m256i* src) {
    int16_t* ptr = (int16_t*) src;
    for (int i = 0; i < 16; i++) { printf("%d ", ptr[i]); }
    printf("\n");
}
void _mm256_print_epi32(__m256i* src) {
    int32_t* ptr = (int32_t*) src;
    for (int i = 0; i < 8; i++) { printf("%d ", ptr[i]); }
    printf("\n");
}


//---------------------------------------------------------------------------//
//                           AVX Boolean functions                           //
//---------------------------------------------------------------------------//

int _mm256_equal_epi32(__m256i* x, __m256i* y) { 
    int32_t* ptr_x = (int32_t*)x;
    int32_t* ptr_y = (int32_t*)y;
    for (int i = 0; i < 8; i += 1) {
        if (*ptr_x != *ptr_y) {
            return 0;
        }
        ptr_x += 1;
        ptr_y += 1;
    }
    return 1;
}


//---------------------------------------------------------------------------//
//                         AVX Special set functions                         //
//---------------------------------------------------------------------------//

void _mm256_arange_epi8(__m256i* x) {
    int8_t* ptr = (int8_t*)x;
    for (int i = 0; i < 32; i++) {
        ptr[i] = i;
    }
}

void _mm256_srange_epi8(__m256i* x) {
    int sign = 1;
    int8_t* ptr = (int8_t*)x;
    for (int i = 0; i < 32; i++) {
        ptr[i] = sign * i;
        sign = -sign; 
    }
}

void _mm256_rand_epi8(__m256i* x) {
    int8_t* ptr = (int8_t*) x;
    for (int i = 0; i < 32; i++) {
        ptr[i] = (int8_t) ((rand() % 256) - 128);
    }
}