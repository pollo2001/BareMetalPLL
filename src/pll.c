#include "helpers.h"
#include "pll.h"

//init pll state
void init_pll(PLL_State* pll){

    //init configs
    pll -> alpha_proporional_gain = DEFAULT_ALPHA_GAIN;
    pll -> beta_integral_gain = DEFAULT_BETA_GAIN;
    pll ->  gamma_derivative_gain = DEFAULT_GAMMA_GAIN;
    pll -> post_divider = 1;  

    //init internal state variables
    pll ->  nco_phase_accumulator = 0;
    pll ->  ref_phase_accumulator = 0;
    pll ->  nco_freq_word = calculate_freq_word(NCO_MIN_FREQUENCY_HZ); //start at minimum of fundamental operating range
    pll ->  n_divider = NCO_MIN_FREQUENCY_HZ / REF_OSCILLATOR_HZ; 

     //init loop filter state variables
    pll -> phase_error = 0; 
    pll ->  loop_integrator = 0; 
    pll -> last_phase_error = 0;
  
    //status INTR FLAG, start no lock 
    pll -> LOCK_FLAG = 0; //lock success indicator, lock = 1

}

//update current state pll
void update_pll();

//compares Vref and Vco (NCO)
uint32_t phase_error_detector(PLL_State* pll);
