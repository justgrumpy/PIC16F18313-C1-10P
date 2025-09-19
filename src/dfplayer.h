/**
 * @file dfplayer.h  
 * @brief DFPlayer Mini audio module control
 *
 * @copyright Copyright (c) 2025 PIC16F18313 i-Bus Audio Controller Project
 * @license MIT License - see LICENSE file for details
 */

#ifndef DFPLAYER_H
#define DFPLAYER_H

#include "config.h"

/**
 * @brief Initialize DFPlayer module
 */
void dfplayer_init(void);

/**
 * @brief Send a string command to DFPlayer
 * @param str Command string to send
 */
void dfplayer_send_string(const char* str);

/**
 * @brief Send a single byte to DFPlayer
 * @param byte Byte to send
 */
void dfplayer_send_byte(char byte);

/**
 * @brief Play startup sound and configure DFPlayer
 */
void dfplayer_startup_sequence(void);

/**
 * @brief Get total number of files (sends query and reads response)
 * @return Number of files on SD card, or 0 if unable to read
 */
uint8_t dfplayer_get_total_files(void);

/**
 * @brief Read response from DFPlayer via software UART on RA2
 * @param buffer Buffer to store response
 * @param max_len Maximum buffer length
 * @return Number of bytes read
 */
uint8_t dfplayer_read_response(char* buffer, uint8_t max_len);

/**
 * @brief Read filename response from DFPlayer, filtering null bytes
 * @param buffer Buffer to store filename
 * @param max_len Maximum buffer length
 * @return Number of bytes read
 */
uint8_t dfplayer_read_filename(char* buffer, uint8_t max_len);

/**
 * @brief Query current playing file number
 */
void dfplayer_query_current_file(void);

/**
 * @brief Play specific file by number
 * @param file_number File number to play (1-based)
 */
void dfplayer_play_file_number(uint8_t file_number);

/**
 * @brief Set DFPlayer volume
 * @param volume Volume level (0-30)
 */
void dfplayer_set_volume(uint8_t volume);

#endif // DFPLAYER_H
