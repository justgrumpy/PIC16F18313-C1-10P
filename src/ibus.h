/**
 * @file ibus.h
 * @brief FlySky i-Bus protocol handling
 *
 * @copyright Copyright (c) 2025 PIC16F18313 i-Bus Audio Controller Project
 * @license MIT License - see LICENSE file for details
 */

#ifndef IBUS_H
#define IBUS_H

#include "config.h"

/**
 * @brief Initialize i-Bus reception
 */
void ibus_init(void);

/**
 * @brief Process incoming i-Bus data and handle switch changes
 */
void process_ibus_input(void);

/**
 * @brief Get channel value for specified channel
 * @param channel Channel number (1-14)
 * @return Channel value (typically 1000-2000)
 */
uint16_t get_channel_value(uint8_t channel);

#endif // IBUS_H
