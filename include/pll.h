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

#include <stdint.h>

//define limits of MCU----assuming 32 bit system
#define SYSTEM_CLOCK_HZ 48000000 //48 Mhz
#define REF_OSCILLATOR_HZ 100000000 //100 MHz

//DEFINE MAX OUTPUTS FREQUENCIES FROM DIVIDERS/SCALARS
#define NCO_MAX_FREQUENCY_HZ 15000000000 //15 GHZ
#define NCO_MIN_FREQUENCY_HZ 7500000000 //7.5 GHz

#define MAX_N_DIVIDER 4095 //in main feedbackc loop (programmable NCO register) 12 bit 
#define MAX_OUTPUT_DIVIDER 1024 //output divider (programmable locked NCO divider regitser) 10 bit


struct PLL_State{

    //config
    uint32_t alpha_proporional_gain; //fixed point: alpha
    uint32_t beta_integral_gain; //fixed point: beta
    uint32_t gamma_derivative_gain; //fixed point: gamma
    uint32_t n_divider; //current N-divider for feedback loop
    uint32_t post_divider; //post-divider for final output, based on locked NCO

    //internal state variables
    uint32_t nco_phase_accumulator; //phase accumulator for reference oscillator
    uint32_t ref_phase_accumulator; //phase accumulator for NCO
    uint32_t nco_freq_word; //direct ouput of loop filter, controls NCO speed

    int32_t phase_error; //error (+/-) in current step of loop, NCO can be ahead or behind reference
    int64_t loop_integrator; //signed, current integrator value, needs to be larger than the error to avoid overflowing after many additions.
    int32_t last_phase_error; //used for derivative calculation
  
    //status INTR FLAG
    volatile uint8_t LOCK_FLAG = 0; //lock success indicator, lock = 1
};

