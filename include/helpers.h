#ifndef helper_h
#define helpers_h

#include <stdint.h>

#define FIXED_POINT_FRACTIONAL_BITS 16 //using Q16.16 format

//multiply two fixed numbers (Q16.16 format)
static inline int32_t fixed_point_product(int32_t a, int32_t b );
static inline uint64_t calculate_freq_word(uint64_t target_freq_hz);
static inline uint64_t reverse_freq_word(uint64_t freq_word);

#endif