#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // for abs()
#include "pll.h"    // Contains the struct and function declarations
#include "helpers.h" // Contains the math helpers

// NOTE: All constants must be available via these headers

int main(void) {
    
    // 1. Setup and Initialization
    PLL_State my_pll;
    init_pll(&my_pll);

    uint64_t target_hz = 12800000000ULL; // Example: 12.8 GHz
    
    printf("--- PLL Simulation Starting ---\n");
    printf("Initial NCO Freq: %llu Hz\n", pll_output_frequency(&my_pll));
    printf("Target Freq: %llu Hz\n", target_hz);
    printf("-------------------------------\n");

    //set the frequency, implicit check inside 
    pll_set_frequency(target_hz, &my_pll);
        printf("New N-Divider Set: %u\n", my_pll.n_divider);
    
    // 2. Simulation Loop (Run for 100 million ticks = ~2 seconds)
    for (uint64_t tick = 0; tick < 100000000ULL; ++tick) {
        update_pll(&my_pll);

        // 3. Logging and Monitoring (Log every 50,000 ticks)
        if (tick % 50000ULL == 0) {
            
            uint64_t current_freq = pll_output_frequency(&my_pll);

            // Check for lock status
            if (my_pll.LOCK_FLAG == 1) {
                printf("\n--- LOCK ACHIEVED! (Tick: %llu) ---\n", tick);
                printf("Final Freq: %llu Hz\n", current_freq);
                // break; // Optionally stop the simulation here
            }

            printf("[T:%llu] Err: %d | Int: %lld | Freq: %llu Hz\n",
                   tick,
                   my_pll.phase_error,
                   my_pll.loop_integrator,
                   current_freq);
            }

    printf("\n--- Simulation Finished ---\n");
    return 0;
        }
    }