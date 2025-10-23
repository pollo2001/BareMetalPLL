#include <stdint.h>

#define FIXED_POINT_FRACTIONAL_BITS 16 //using Q16.16 format

//multiply two fixed numbers (Q16.16 format)
int32_t fixed_point_product(int32_t a, int32_t b );
uint64_t calculate_freq_word(uint64_t target_freq_hz);