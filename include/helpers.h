#include <stdint.h>

#define FIXED_POINT_FRACTIONAL_BITS 16 //using Q16.16 format

//multiply two fixed numbers (Q16.16 format)
int32_t fixed_point_product(int32_t a, int32_t b ){
   
    int64_t temp = (int64_t)a * b; //cast 'a' to 64 bit first, product results in 64 bit

    return (int32_t)(temp >> FIXED_POINT_FRACTIONAL_BITS); //shift by 16 bits right, then cast to 32 bit
}