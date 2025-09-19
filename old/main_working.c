 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

/*
� [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"

/*
    Main application
*/

// i-Bus protocol definitions
#define IBUS_PACKET_SIZE 32
#define IBUS_HEADER1 0x20
#define IBUS_HEADER2 0x40
#define IBUS_CHANNEL_COUNT 14

// Switch states for channel 5
#define SWITCH_UP_VALUE 1000    // 0x03E8
#define SWITCH_DOWN_VALUE 2000  // 0x07D0

// i-Bus packet buffer
uint8_t ibus_packet[IBUS_PACKET_SIZE];

// Ring buffer for interrupt-driven UART reception
#define RING_BUFFER_SIZE 64
volatile uint8_t ring_buffer[RING_BUFFER_SIZE];
volatile uint8_t buffer_head = 0;
volatile uint8_t buffer_tail = 0;

// Function declarations
uint8_t read_ibus_packet(void);
uint16_t get_channel_value(uint8_t channel);
void process_ibus_input(void);
uint8_t ring_buffer_available(void);
uint8_t ring_buffer_read(void);

// Helper functions for DFPlayer communication
void dfplayer_send_byte(uint8_t data) {
    while (!EUSART_IsTxReady()); // Wait until TX buffer is ready
    EUSART_Write(data);
}

void dfplayer_send_string(const char *str) {
    while (*str) {
        dfplayer_send_byte(*str++);
    }
}

// Debug output functions
void debug_send_hex_byte(uint8_t value) {
    const char hex_chars[] = "0123456789ABCDEF";
    dfplayer_send_byte(hex_chars[(value >> 4) & 0x0F]);
    dfplayer_send_byte(hex_chars[value & 0x0F]);
}

// Ring buffer helper functions
uint8_t ring_buffer_available(void) {
    return (buffer_head != buffer_tail);
}

uint8_t ring_buffer_read(void) {
    uint8_t data;
    if (buffer_head != buffer_tail) {
        data = ring_buffer[buffer_tail];
        buffer_tail = (buffer_tail + 1) % RING_BUFFER_SIZE;
        return data;
    }
    return 0; // Should not happen if ring_buffer_available() is checked first
}

// UART RX Interrupt Service Routine
void __interrupt() ISR(void) {
    // Handle UART RX interrupt
    if (PIE1bits.RCIE && PIR1bits.RCIF) {
        // Read the received byte
        uint8_t received_byte = RCREG1;
        
        // Store in ring buffer
        uint8_t next_head = (buffer_head + 1) % RING_BUFFER_SIZE;
        if (next_head != buffer_tail) {
            // Buffer not full, store the byte
            ring_buffer[buffer_head] = received_byte;
            buffer_head = next_head;
        }
        // If buffer full, just discard byte
        
        // Clear the interrupt flag
        PIR1bits.RCIF = 0;
    }
}

// Ultra-simple i-Bus packet reading - just find header and read 32 bytes
uint8_t read_ibus_packet(void) {
    static uint8_t looking_for_header = 1;
    static uint8_t packet_pos = 0;
    uint8_t byte_val;

    // Data arrives via interrupts, no need to poll
    
    // Process bytes one at a time from ring buffer
    while (ring_buffer_available() > 0) {
        byte_val = ring_buffer_read();
        
        if (looking_for_header) {
            // Look for 0x20 0x40 sequence
            if (packet_pos == 0 && byte_val == IBUS_HEADER1) {
                ibus_packet[0] = byte_val;
                packet_pos = 1;
            } else if (packet_pos == 1 && byte_val == IBUS_HEADER2) {
                ibus_packet[1] = byte_val;
                packet_pos = 2;
                looking_for_header = 0; // Now read the rest
            } else {
                // Reset if we don't get the right sequence
                packet_pos = 0;
                if (byte_val == IBUS_HEADER1) {
                    ibus_packet[0] = byte_val;
                    packet_pos = 1;
                }
            }
        } else {
            // Reading packet data
            ibus_packet[packet_pos] = byte_val;
            packet_pos++;
            
            if (packet_pos >= IBUS_PACKET_SIZE) {
                // Got full packet - validate it doesn't have embedded headers
                uint8_t valid = 1;
                uint8_t i;
                
                // Check for any embedded 0x20 0x40 sequences in positions 2-30
                for (i = 2; i < IBUS_PACKET_SIZE - 1; i++) {
                    if (ibus_packet[i] == 0x20 && ibus_packet[i+1] == 0x40) {
                        valid = 0;
                        break;
                    }
                }
                
                looking_for_header = 1;
                packet_pos = 0;
                
                if (valid) {
                    return 1; // Return only clean packets
                }
                // If invalid, continue looking for next packet
            }
        }
    }
    
    return 0;
}

uint16_t get_channel_value(uint8_t channel) {
    uint8_t byte_index;
    uint16_t value;
    
    if (channel < 1 || channel > IBUS_CHANNEL_COUNT) return 1500;  // Invalid channel
    
    // Calculate byte index (channel 1 starts at byte 2)
    byte_index = 2 + ((channel - 1) * 2);
    
    // Combine low and high bytes (low byte first in i-Bus)
    value = (uint16_t)ibus_packet[byte_index] | ((uint16_t)ibus_packet[byte_index + 1] << 8);
    
    return value;
}

void process_ibus_input(void) {
    static uint16_t last_ch5_value = 0; // Track the actual value, not a state
    
    if (!read_ibus_packet()) return;
    
    uint16_t ch5_value = get_channel_value(5);
    
    // Skip action on first run or if value hasn't changed
    if (last_ch5_value == 0 || ch5_value == last_ch5_value) {
        last_ch5_value = ch5_value;
        return;
    }
    
    // Play appropriate file based on new value
    if (ch5_value == SWITCH_UP_VALUE) {
        dfplayer_send_string("AT+PLAYFILE=/mp3/0002.mp3\r\n");
    } else if (ch5_value == SWITCH_DOWN_VALUE) {
        dfplayer_send_string("AT+PLAYFILE=/mp3/0003.mp3\r\n");
    }
    
    last_ch5_value = ch5_value;
    __delay_ms(2000);
}

int main(void){
    SYSTEM_Initialize();
    
    // Ensure EUSART receiver is properly enabled
    EUSART_Enable();         // Enable the entire EUSART module
    EUSART_ReceiveEnable();  // Explicitly enable receiver
    
    // Enable UART RX interrupts for true interrupt-driven reception
    PIE1bits.RCIE = 1;       // Enable UART RX interrupt
    INTCONbits.PEIE = 1;     // Enable peripheral interrupts
    INTCONbits.GIE = 1;      // Enable global interrupts
    
    // Clear any existing overrun errors
    if (RC1STAbits.OERR) {
        RC1STAbits.CREN = 0;  // Disable continuous receive
        RC1STAbits.CREN = 1;  // Re-enable continuous receive
    }
    
    // Wait for DFPlayer to initialize
    __delay_ms(3000);  // 3 second delay for DFPlayer startup
    
    // Configure DFPlayer settings
    dfplayer_send_string("AT+PROMPT=OFF\r\n");  // Turn off voice prompts
    __delay_ms(1000);
    
    dfplayer_send_string("AT+LED=OFF\r\n");     // Turn off LED indicator
    __delay_ms(1000);

    dfplayer_send_string("AT+VOL=27\r\n");      // Set volume to 27
    __delay_ms(1000);
    
    // Play the startup file
    dfplayer_send_string("AT+PLAYFILE=/mp3/0001.mp3\r\n");
    __delay_ms(3000);  // Wait for startup sound to finish
    
    while(1) {
        static uint16_t heartbeat_counter = 0;
        
        // Continuously monitor i-Bus input and handle switch changes
        process_ibus_input();
        
        // Use faster polling to keep up with data rate
        __delay_ms(1); // 1ms delay for faster polling
    }    
}