/**
 * @file main.c
 * @brief Main application entry point and system coordination
 * 
 * This file contains the main initialization sequence and application loop.
 * The actual functionality is implemented in separate modules:
 * - dfplayer.c: Audio control via DFPlayer Mini
 * - ibus.c: FlySky i-Bus protocol handling  
 *
 * @copyright Copyright (c) 2025 PIC16F18313 i-Bus Audio Controller Project
 * @license MIT License - see LICENSE file for details
 */

#include "mcc_generated_files/system/system.h"
#include "src/config.h"
#include "src/dfplayer.h"
#include "src/ibus.h"

/**
 * @brief Main application entry point
 */
int main(void) {
    // Initialize MCC generated system
    SYSTEM_Initialize();
    
    // Initialize application modules
    dfplayer_init();
    ibus_init();
    
    // Configure and play startup sequence
    dfplayer_startup_sequence();
    
    // Main application loop
    while (1) {
        // Continuously monitor i-Bus input and handle switch changes
        process_ibus_input();
        
        // Use faster polling to keep up with data rate
        __delay_ms(1); // 1ms delay for faster polling
    }    
    
    return 0;
}
