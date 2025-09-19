/**
 * @file dfplayer.c
 * @brief DFPlayer Mini audio module control implementation
 *
 * @copyright Copyright (c) 2025 PIC16F18313 i-Bus Audio Controller Project
 * @license MIT License - see LICENSE file for details
 */

#include "dfplayer.h"
#include "../mcc_generated_files/system/system.h"
#include "../mcc_generated_files/timer/delay.h"

// Forward declaration for static function
static uint8_t dfplayer_read_byte(void);
static void dfplayer_send_number(uint8_t number);

void dfplayer_init(void) {
    // DFPlayer is initialized through EUSART, no additional setup needed
    // RA2 is already configured as digital input with pull-up via MCC
}

// Software UART receiver for DFPlayer responses on RA2
// Simple implementation for 9600 baud (104μs per bit)
uint8_t dfplayer_read_response(char* buffer, uint8_t max_len) {
    uint8_t byte_count = 0;
    uint16_t timeout_counter;
    
    // Wait for start bit (falling edge) with timeout
    timeout_counter = 0;
    while (IO_RA2_GetValue() == 1 && timeout_counter < 10000) {
        DELAY_microseconds(10);
        timeout_counter++;
    }
    
    if (timeout_counter >= 10000) return 0; // Timeout - no data
    
    // Read bytes until we get \r\n or buffer full
    while (byte_count < (max_len - 1)) {
        uint8_t received_byte = dfplayer_read_byte();
        
        if (received_byte == 0) break; // Error or timeout
        
        buffer[byte_count] = received_byte;
        byte_count++;
        
        // Check for end of response
        if (byte_count >= 2 && 
            buffer[byte_count-2] == '\r' && 
            buffer[byte_count-1] == '\n') {
            break;
        }
    }
    
    buffer[byte_count] = '\0'; // Null terminate
    return byte_count;
}

// Read a single byte using software UART on RA2 (9600 baud)
static uint8_t dfplayer_read_byte(void) {
    uint8_t byte = 0;
    uint8_t bit_count;
    
    // Wait for start bit (should already be low)
    if (IO_RA2_GetValue() == 1) return 0; // No start bit
    
    DELAY_microseconds(52); // Wait to center of start bit (half bit time)
    
    // Sample 8 data bits (LSB first)
    for (bit_count = 0; bit_count < 8; bit_count++) {
        DELAY_microseconds(104); // One bit time at 9600 baud
        
        if (IO_RA2_GetValue()) {
            byte |= (1 << bit_count); // Set bit if line is high
        }
    }
    
    DELAY_microseconds(104); // Wait through stop bit
    
    return byte;
}

// Helper function to send a number as ASCII digits (no zero padding)
static void dfplayer_send_number(uint8_t number) {
    char buffer[4]; // Max 3 digits + null terminator
    uint8_t i = 0;
    
    // Convert number to string (simple approach)
    if (number == 0) {
        dfplayer_send_byte('0');
    } else {
        // Build string in reverse, then send forward
        uint8_t temp = number;
        while (temp > 0) {
            buffer[i++] = '0' + (temp % 10);
            temp /= 10;
        }
        // Send digits in correct order (reverse of how we built them)
        while (i > 0) {
            dfplayer_send_byte(buffer[--i]);
        }
    }
}

// Read filename response, filtering out null bytes
uint8_t dfplayer_read_filename(char* buffer, uint8_t max_len) {
    uint8_t byte_count = 0;
    uint16_t timeout_counter;
    
    // Wait for start bit (falling edge) with timeout
    timeout_counter = 0;
    while (IO_RA2_GetValue() == 1 && timeout_counter < 10000) {
        DELAY_microseconds(10);
        timeout_counter++;
    }
    
    if (timeout_counter >= 10000) return 0; // Timeout - no data
    
    // Read bytes until we get \r\n or buffer full
    while (byte_count < (max_len - 1)) {
        uint8_t received_byte = dfplayer_read_byte();
        
        if (received_byte == 0) {
            // Skip null bytes - they might be padding or Unicode encoding
            continue;
        }
        
        buffer[byte_count] = received_byte;
        byte_count++;
        
        // Check for end of response
        if (byte_count >= 2 && 
            buffer[byte_count-2] == '\r' && 
            buffer[byte_count-1] == '\n') {
            break;
        }
    }
    
    buffer[byte_count] = '\0'; // Null terminate
    return byte_count;
}

void dfplayer_send_string(const char* str) {
    while (*str) {
        while (!EUSART_IsTxReady());
        EUSART_Write(*str++);
    }
}

void dfplayer_send_byte(char byte) {
    while (!EUSART_IsTxReady());
    EUSART_Write(byte);
}

void dfplayer_startup_sequence(void) {
    DELAY_milliseconds(DFPLAYER_STARTUP_DELAY);

    // Configure DFPlayer settings
    dfplayer_send_string("AT+LED=OFF\r\n");     // Turn off LED indicator
    DELAY_milliseconds(1000);

    dfplayer_send_string("AT+VOL=");
    dfplayer_send_number(DFPLAYER_VOLUME_DEFAULT);
    dfplayer_send_string("\r\n");
    DELAY_milliseconds(1000);

    // Set to play one song and pause
    dfplayer_send_string("AT+PLAYMODE=3\r\n");
    DELAY_milliseconds(1000);
    
    // Play the startup file
    // dfplayer_send_string("AT+PLAYFILE=/startup.mp3\r\n");
    dfplayer_play_file_number(1);
    DELAY_milliseconds(2000);
}

uint8_t dfplayer_get_total_files(void) {
    char response[16];
    uint8_t len;
    
    // Send query command
    dfplayer_send_string("AT+QUERY=2\r\n");
    DELAY_milliseconds(100); // Give DFPlayer time to respond
    
    // Read response
    len = dfplayer_read_response(response, sizeof(response));
    
    if (len > 0) {
        // Parse simple response format: "15\r\n"
        // Convert string to number
        uint8_t file_count = 0;
        char* ptr = response;
        
        // Skip any leading whitespace or non-digit characters
        while (*ptr && (*ptr < '0' || *ptr > '9')) {
            ptr++;
        }
        
        // Extract number
        while (*ptr >= '0' && *ptr <= '9') {
            file_count = file_count * 10 + (*ptr - '0');
            ptr++;
        }
        
        return file_count;
    }
    
    return 0; // Could not determine file count
}

void dfplayer_query_current_file(void) {
    dfplayer_send_string("AT+QUERY=1\r\n");
}

void dfplayer_play_file_number(uint8_t file_number) {
    dfplayer_send_string("AT+PLAYNUM=");
    dfplayer_send_number(file_number);
    dfplayer_send_string("\r\n");
}

void dfplayer_set_volume(uint8_t volume) {
    if (volume > 30) volume = 30;  // Clamp to maximum volume
    dfplayer_send_string("AT+VOL=");
    dfplayer_send_number(volume);
    dfplayer_send_string("\r\n");
}
