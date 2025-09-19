/**
 * @file config.h
 * @brief Common configuration constants and definitions
 *
 * @copyright Copyright (c) 2025 PIC16F18313 i-Bus Audio Controller Project
 * @license MIT License - see LICENSE file for details
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// Note: _XTAL_FREQ is already defined by MCC in clock.h

// i-Bus configuration
#define IBUS_BUFFER_SIZE 32
#define IBUS_PACKET_SIZE 32
#define IBUS_CHANNELS 14

// DFPlayer configuration
#define DFPLAYER_VOLUME_DEFAULT 6
#define DFPLAYER_STARTUP_DELAY 3000

#endif // CONFIG_H
