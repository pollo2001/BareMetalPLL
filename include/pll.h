/*PLL State machine using fixed point...
Blocks are as so:
                  +--------------------+
                  |                    |
Reference --------> |   Phase Detector   | ----> Loop Filter ----> VCO/NCO ----> Output
                  |                    |                       |
                  +--------------------+                       |
                          ^                                    |
                          |                                    |
                          +-------- Frequency Divider <--------+
*/
#ifndef pll_h
#define pll_h

#include <stdint.h>

//define limits of MCU----assuming 32 bit system
#define SYSTEM_CLOCK_HZ 48000000 //48 Mhz
#define REF_OSCILLATOR_HZ 10000000 //10 MHz
//calculate the fixed phase step for the reference clock
#define REF_STEP_VALUE ( ( ( (uint64_t)REF_OSCILLATOR_HZ * PHASE_ACCUMULATOR_MAX ) / SYSTEM_CLOCK_HZ ) )
#define PHASE_ACCUMULATOR_MAX 0xFFFFFFFFUL // Represents the max value of a 32-bit unsigned int (2^32 - 1)

//DEFINE MAX OUTPUTS FREQUENCIES FROM DIVIDERS/SCALARS
#define NCO_MAX_FREQUENCY_HZ 15000000000 //15 GHZ
#define NCO_MIN_FREQUENCY_HZ 7500000000 //7.5 GHz

//MAX/MIN OUTPUT FREQUENCIES
#define MAX_OUTPUT_FREQUENCY 30000000000 //30 GHz
#define MIN_OUTPUT_FREQUENCY 30000000 //30 MHz

#define MAX_N_DIVIDER 4095 //in main feedbackc loop (programmable NCO register) 12 bit 
#define MAX_OUTPUT_DIVIDER 1024 //output divider (programmable locked NCO divider regitser) 10 bit

//default PID values
#define DEFAULT_ALPHA_GAIN 6553 //Kp = (.1 * (1 << 16)), Q.16.16
#define DEFAULT_BETA_GAIN  327  //(0.005 * (1 << 16))
#define DEFAULT_GAMMA_GAIN 0    // we set the D term to 0, it should not be present in reacting to jitter (noise change)

typedef struct PLL_State{
    //config
    int32_t alpha_proporional_gain;    //fixed point: alpha
    int32_t beta_integral_gain;    //fixed point: beta
    int32_t gamma_derivative_gain; //fixed point: gamma
    uint32_t n_divider; //current N-divider for feedback loop
    uint32_t post_divider;  //post-divider for final output, based on locked NCO
    uint64_t target_frequency_label; //the user input tagert to compare

    //internal state variables
    uint32_t nco_phase_accumulator; //phase accumulator for NCO hase accumulator for NCOphase accumulator for reference oscillator
    uint32_t ref_phase_accumulator; //phase accumulator for NCOphase accumulator for reference oscillator
    uint64_t nco_freq_word; //direct ouput of loop filter, controls NCO speed
    uint64_t base_nco_freq_word;

    //loop filter state variables
    int32_t phase_error; //error (+/-) in current step of loop, NCO can be ahead or behind reference
    int64_t loop_integrator; //signed, current integrator value, needs to be larger than the error to avoid overflowing after many additions.
    int32_t last_phase_error; //used for derivative calculation
  
    //status INTR FLAG
    volatile uint8_t LOCK_FLAG; //lock success indicator, lock = 1
} PLL_State;

//init pll state, runs once
void init_pll(PLL_State *pll);

//update current state pll, constantly in the loop
void update_pll(PLL_State *pll);

//compares Vref and Vco (NCO), called alot, so inline to eliminate overhead
static inline void phase_error_detector(PLL_State *pll);

//PID controller
void pll_loop_filter(PLL_State *pll);

//set the dividers and frequency word based off the target frequency
void pll_set_frequency(uint32_t target_frequency, PLL_State *pll);

uint64_t pll_output_frequency(PLL_State *pll);

#endif