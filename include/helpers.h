#ifndef helpers_h
#define helpers_h

#include <stdint.h>
#include "pll.h"

#define FIXED_POINT_FRACTIONAL_BITS 16 //using Q16.16 format
//multiply two fixed numbers (Q16.16 format)
static inline int32_t fixed_point_product(int32_t a, int32_t b ){
   
    int64_t temp = (int64_t)a * b; //cast 'a' to 64 bit first, product results in 64 bit

    return (int32_t)(temp >> FIXED_POINT_FRACTIONAL_BITS); //shift by 16 bits right, then cast to 32 bit
}

//calculates the NCO frequency control word, from a target frequency in Hz.
static inline uint64_t calculate_freq_word(uint64_t target_freq_hz){

    //get fre_word integer
    uint64_t freq_word = (target_freq_hz / SYSTEM_CLOCK_HZ) * PHASE_ACCUMULATOR_MAX;

    // then, do the fractional part
    uint64_t remainder = target_freq_hz % SYSTEM_CLOCK_HZ;
    freq_word += (remainder * PHASE_ACCUMULATOR_MAX) / SYSTEM_CLOCK_HZ;

    return freq_word;
}

//calculates the frequency in Hz from a frequency word number
static inline uint64_t reverse_freq_word(uint64_t freq_word){
    // use the highest 32 bits of freq_word, multiply by SYS_CLK, and divide by PHASE_MAX (2^32).
    // equivalent to (freq_word / 2^32) * SYS_CLK.
    uint64_t hz = (freq_word >> 32) * SYSTEM_CLOCK_HZ;

    // calculate the precision/remainder part.
    // use the lower 32 bits of freq_word.
    uint64_t remainder_word = freq_word & 0xFFFFFFFFUL; 
    
    // add the fractional contribution
    hz += (remainder_word * SYSTEM_CLOCK_HZ) / PHASE_ACCUMULATOR_MAX;
    
    return hz;
}

#endif