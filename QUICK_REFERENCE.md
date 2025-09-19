# Quick Reference Guide

## Project Overview
PIC16F18313 i-Bus receiver with audio control via DFPlayer Mini. Features multi-channel sound effects and real-time volume control.

## Key Files

| File | Purpose |
|------|---------|
| `main.c` | System coordination and main loop |
| `src/ibus.c` | **Ring buffer + interrupt-driven i-Bus reception** |
| `src/dfplayer.c` | Audio control via UART commands |
| `src/config.h` | System constants |
| `README.md` | Complete documentation with diagrams |
| `SCHEMATIC.md` | Circuit diagram and connections |
| `LICENSE` | MIT License |

## Critical Design Decision

### ❌ Don't Use Polling (MCC EUSART functions for RX)
```c
// This WILL FAIL for continuous i-Bus streams:
if (EUSART_IsRxReady()) {
    uint8_t byte = EUSART_Read();
    // By the time you get here, 3-4 more bytes arrived and were lost!
}
```

### ✅ Use Interrupt + Ring Buffer (Current Implementation)
```c
// This WORKS for continuous i-Bus streams:
void __interrupt() ISR(void) {
    if (PIR1bits.RCIF) {
        ring_buffer[buffer_head] = RCREG1;  // Capture immediately
        buffer_head = (buffer_head + 1) % 64;
    }
}
```

## System Flow

```
RC Transmitter → i-Bus → UART RX → ISR → Ring Buffer → Packet Parser → Audio Control → DFPlayer
```

## Pin Configuration
- **RA0**: i-Bus RX (115200 baud)
- **RA1**: DFPlayer TX (115200 baud)
- **RA2**: DFPlayer RX (software UART for responses)

## Audio Control Mapping

### Channel 5 (Switch) - Cycling Sound Effects
Each switch position change cycles through:
1. tada.mp3
2. 3wah.mp3
3. exclaim.mp3
4. growl.mp3
5. okay.mp3
6. yes.mp3

### Channel 6 (Switch) - Grumble Sounds
Each switch position change cycles through:
1. grumbl02.mp3
2. grumbl03.mp3
3. grumbl04.mp3
4. grumbl05.mp3

### Channel 7 (Pot) - Volume Control
- **1000**: Volume 0 (silent)
- **2000**: Volume 30 (maximum)
- **Linear mapping**: Real-time volume adjustment

## Build Instructions
1. Open `uart.X` in MPLAB X
2. Ensure XC8 compiler is selected
3. Build project (compiles `main.c` and all `src/` modules)
4. Program PIC16F18313

## Key Functions

### i-Bus Functions
```c
void ibus_init(void);              // Enable RX interrupts
void process_ibus_input(void);     // Parse packets, handle Ch5/Ch6/Ch7
uint16_t get_channel_value(uint8_t channel); // Extract channel data (1-14)
```

### DFPlayer Functions  
```c
void dfplayer_init(void);          // Initialize audio system
void dfplayer_startup_sequence(void); // Volume + startup delay
void dfplayer_send_string(const char* str); // Send AT commands
void dfplayer_set_volume(uint8_t volume);   // Set volume (0-30)
```

## Required SD Card Files
```
/tada.mp3       (Channel 5 - file 1)
/3wah.mp3       (Channel 5 - file 2)
/exclaim.mp3    (Channel 5 - file 3)
/growl.mp3      (Channel 5 - file 4)
/okay.mp3       (Channel 5 - file 5)
/yes.mp3        (Channel 5 - file 6)
/grumbl02.mp3   (Channel 6 - file 1)
/grumbl03.mp3   (Channel 6 - file 2)
/grumbl04.mp3   (Channel 6 - file 3)
/grumbl05.mp3   (Channel 6 - file 4)
```

## Ring Buffer Internals
- **Size**: 64 bytes (handles 2 full packets)
- **ISR**: Writes to `buffer_head`
- **Main**: Reads from `buffer_tail`
- **Thread-safe**: No shared write locations

---

**Remember**: The ring buffer is the heart of this system. Without it, the continuous i-Bus data stream will be corrupted and the system will fail. This is not optional - it's essential for reliable operation.
