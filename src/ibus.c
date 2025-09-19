/**
 * @file ibus.c
 * @brief FlySky i-Bus protocol handling implementation
 *
 * @copyright Copyright (c) 2025 PIC16F18313 i-Bus Audio Controller Project
 * @license MIT License - see LICENSE file for details
 */

#include "ibus.h"
#include "dfplayer.h"
#include "../mcc_generated_files/system/system.h"
#include "../mcc_generated_files/timer/delay.h"

// i-Bus packet structure constants
#define IBUS_HEADER1 0x20
#define IBUS_HEADER2 0x40
#define IBUS_CHANNEL_COUNT 14

// Switch states for channel 5
#define SWITCH_UP_VALUE 1000
#define SWITCH_DOWN_VALUE 2000

// Ring buffer for UART RX data
#define RING_BUFFER_SIZE 64
static volatile uint8_t ring_buffer[RING_BUFFER_SIZE];
static volatile uint8_t buffer_head = 0;
static volatile uint8_t buffer_tail = 0;

// i-Bus packet storage
static uint8_t ibus_packet[IBUS_PACKET_SIZE];

// Ring buffer helper functions
static uint8_t ring_buffer_available(void) {
    return (buffer_head != buffer_tail);
}

static uint8_t ring_buffer_read(void) {
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
static uint8_t read_ibus_packet(void) {
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

void ibus_init(void) {
    // Enable UART RX interrupt
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
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
    static uint16_t last_ch5_value = 0;
    static uint16_t last_ch6_value = 0;
    static uint16_t last_ch7_value = 0;  // For volume control
    static uint8_t ch5_file_index = 0;  // Current file index for channel 5
    static uint8_t ch6_file_index = 0;  // Current file index for channel 6
    
    // Channel 5 file list (6 files)
    static const char* ch5_files[] = {
        "AT+PLAYFILE=/tada.mp3\r\n",
        "AT+PLAYFILE=/3wah.mp3\r\n", 
        "AT+PLAYFILE=/exclaim.mp3\r\n",
        "AT+PLAYFILE=/growl.mp3\r\n",
        "AT+PLAYFILE=/okay.mp3\r\n",
        "AT+PLAYFILE=/yes.mp3\r\n"
    };
    
    // Channel 6 file list (4 files)
    static const char* ch6_files[] = {
        "AT+PLAYFILE=/grumbl02.mp3\r\n",
        "AT+PLAYFILE=/grumbl03.mp3\r\n",
        "AT+PLAYFILE=/grumbl04.mp3\r\n",
        "AT+PLAYFILE=/grumbl05.mp3\r\n"
    };

    if (!read_ibus_packet()) return;
    
    uint16_t ch5_value = get_channel_value(5);
    uint16_t ch6_value = get_channel_value(6);
    uint16_t ch7_value = get_channel_value(7);
    
    // Handle channel 5 changes
    if (last_ch5_value != 0 && ch5_value != last_ch5_value) {
        // Play current file and advance to next
        dfplayer_send_string(ch5_files[ch5_file_index]);
        ch5_file_index = (ch5_file_index + 1) % 6;  // Wrap around after 6 files
        DELAY_milliseconds(100);  // Small delay between commands
    }
    
    // Handle channel 6 changes
    if (last_ch6_value != 0 && ch6_value != last_ch6_value) {
        // Play current file and advance to next
        dfplayer_send_string(ch6_files[ch6_file_index]);
        ch6_file_index = (ch6_file_index + 1) % 4;  // Wrap around after 4 files
        DELAY_milliseconds(100);  // Small delay between commands
    }
    
    // Handle channel 7 volume control (pot)
    if (last_ch7_value != ch7_value) {
        // Map channel value (1000-2000) to volume (0-30)
        uint8_t volume;
        if (ch7_value <= 1000) {
            volume = 0;
        } else if (ch7_value >= 2000) {
            volume = 30;
        } else {
            // Linear interpolation: (ch7_value - 1000) * 30 / (2000 - 1000)
            volume = ((uint32_t)(ch7_value - 1000) * 30) / 1000;
        }
        
        dfplayer_set_volume(volume);
        DELAY_milliseconds(50);  // Short delay for volume command
    }
    
    // Update last values
    last_ch5_value = ch5_value;
    last_ch6_value = ch6_value;
    last_ch7_value = ch7_value;
}
