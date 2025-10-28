

#include "helpers.h"
#include "pll.h"

//init pll state
void init_pll(PLL_State* pll){

    //init configs
    pll -> alpha_proporional_gain = DEFAULT_ALPHA_GAIN;
    pll -> beta_integral_gain = DEFAULT_BETA_GAIN;
    pll -> gamma_derivative_gain = DEFAULT_GAMMA_GAIN;
    pll -> post_divider = 1;  

    //init internal state variables
    pll -> nco_phase_accumulator = 0;
    pll -> ref_phase_accumulator = 0;
    pll -> nco_freq_word = calculate_freq_word(NCO_MIN_FREQUENCY_HZ);; //start at minimum of fundamental operating range
    pll -> base_nco_freq_word = pll-> nco_freq_word; //base frequency word for prevneitng runway loop
    pll -> n_divider = NCO_MIN_FREQUENCY_HZ / REF_OSCILLATOR_HZ; 

     //init loop filter state variables
    pll -> phase_error = 0; 
    pll -> loop_integrator = 0; 
    pll -> last_phase_error = 0; //D is 0, can be used for expansion later
  
    //status INTR FLAG, start no lock 
    pll -> LOCK_FLAG = 0; //lock success indicator, lock = 1
    pll -> target_frequency_label = 0; //none, only set when N divide exists
}

//update current state pll
void update_pll(PLL_State *pll){
    //****always running****
    uint32_t previous_phase_accumulator = pll -> ref_phase_accumulator;
    //advance the NCO clock
    pll -> nco_phase_accumulator += pll-> nco_freq_word;
    //advance the ref clock
    pll -> ref_phase_accumulator += REF_STEP_VALUE;

    //check if reference clock ticks (overflow)
    //checking the if uint32 overflow, occurs when new value smaller than previous value
    if(pll -> ref_phase_accumulator < previous_phase_accumulator){
        phase_error_detector(pll); //find phase error
        loop_filter(pll); //upate NCO speed
    }

    else 
    return; 
}

//compares Vref and Vco (NCO)
void pll_phase_error_detector(PLL_State *pll){
    //return the difference of these, NCO may be ahead or behind
   pll -> phase_error = pll ->ref_phase_accumulator - pll -> nco_phase_accumulator;
}

//take in phase error and calculate new nco_freq_word using PID, D = 0
void pll_loop_filter(PLL_State *pll){
    //Linear PID equation:
    //prevent overflow with fix point math
    //errors must be signed

    //proportional output: P_out = Kp * phase_error
   int32_t P_out = fixed_point_product(pll -> alpha_proporional_gain, pll -> phase_error);
    //integral output: I_out = I_out + (Ki * phase_error)
   pll -> loop_integrator = pll -> loop_integrator + fixed_point_product(pll-> beta_integral_gain, pll -> phase_error);
   int32_t I_out = pll -> loop_integrator; //new integrator value

   //derriavtive output: D_out = Kd *  to expand on later if derirvative considered
   // uint32_t D_out = fixed_point_product(pll -> gamma_derivative_gain, pll -> phase_error);
   int32_t total_correction = P_out + I_out; //+ D_out
   
   //update new nco_freq_word by adding the correction calculated 
   //corrects the base word of the current target frequency, staying on target
   pll -> nco_freq_word = pll -> base_nco_freq_word + total_correction;
}

//get target frequency from client and divide to find mutliplier/dividers and set the NCO
void pll_set_frequency(uint32_t target_frequency, PLL_State *pll){
    //check that target frequency is in bounds of Output frequency
    if (target_frequency > MAX_OUTPUT_FREQUENCY || target_frequency < MIN_OUTPUT_FREQUENCY){
        printf("ERROR: Target frequency %u out of range, MIN: %llu\nMAX:%llu\n", 
            target_frequency, MIN_OUTPUT_FREQUENCY, MAX_OUTPUT_FREQUENCY);
        return;
    }

    //then find the nco target frequency, to determine the n divider for the feedback
    // ... (logic for Cases 1, 2, and 3) ...
    uint64_t nco_target_frequency = 0;

    //case 1: in bounds of the nco range
    if (target_frequency > NCO_MAX_FREQUENCY_HZ){
        nco_target_frequency = target_frequency / 2; //needs to be doubled
        pll -> post_divider = 0; //need to mulitply so will send to multiplier with post divider at 0 as flag..
    }
    //case 2: target is inside NCO range
    else if (target_frequency >= NCO_MIN_FREQUENCY_HZ){
        nco_target_frequency = target_frequency;
        pll -> post_divider = 1; //divide by 1 , it is itself
    }
    //case 3: less than nco range
    else {
         pll -> post_divider = (NCO_MIN_FREQUENCY_HZ + target_frequency - 1) / target_frequency;
        nco_target_frequency = (uint64_t)target_frequency * pll-> post_divider;
    }
    
    //run the safety check on the *result*
    if (nco_target_frequency > NCO_MAX_FREQUENCY_HZ || nco_target_frequency < NCO_MIN_FREQUENCY_HZ) {
        printf("ERROR: Cannot lock. Calculated NCO frequency is out of range.\n");
        return; // Or return false
    }

    //calculate the n divider; N = F_tszarget / F_ref, only if the results is perfect integer
    if(nco_target_frequency % REF_OSCILLATOR_HZ != 0){
        printf("ERROR:Frequency not valid, need N-fractional PLL, Choose integer multiple frequencies of the reference\n");
        pll -> n_divider = 1; //to not crash everything
        return;
    }
    else{
        pll -> target_frequency_label = target_frequency;
        pll -> n_divider = nco_target_frequency / REF_OSCILLATOR_HZ; //valid frequency based off n divider
        pll -> base_nco_freq_word = calculate_freq_word(nco_target_frequency); // base for this frequency
        pll -> loop_integrator = 0; //reset error values
        //this feeds into loop filter
    }
   return;
}

//sets the output frequency and sets the lock
uint64_t pll_output_frequency(PLL_State *pll)
{
    uint64_t current_nco_freq = reverse_freq_word(pll -> nco_freq_word);

    //only two cases to chekc based of off post divider logically
    if(!pll -> post_divider){ //doubled case
        return current_nco_freq * 2; //doubler
    }
    else //nco is in range, divide by current post divider
        return current_nco_freq / pll -> post_divider;
}

//implement lock check