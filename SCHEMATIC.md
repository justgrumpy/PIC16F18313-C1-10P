# Circuit Schematic: PIC16F18313 i-Bus Audio Controller

## Component Overview
- **Microcontroller**: PIC16F18313 (8-pin PDIP)
- **Audio Module**: DFPlayer Pro
- **Receiver**: FlySky FS-iA6B (i-Bus protocol)
- **Decoupling Capacitors**: 10µF 25V (PIC), 22µF 25V (DFPlayer)
- **Protection Resistor**: 1kΩ (PIC TX to DFPlayer RX)

## Pin Connections

### PIC16F18313 Pinout (8-pin PDIP)
```
     PIC16F18313
    ┌───────────┐
VDD │1         8│ VSS (GND)
RA5 │2         7│ RA0/ICSPDAT
RA4 │3         6│ RA1/ICSPCLK
RA3 │4         5│ RA2
    └───────────┘
```

### Connection Diagram

```
                     +5V Power Supply
                             │
          ┌──────────────────┴──────────────────┐
          │                                     │
          │     ┌───────┐            ┌───────┐  │
          │     │  C1   │            │  C2   │  │
          │     │ 10µF  │            │ 22µF  │  │
          │     │  25V  │            │  25V  │  │
          │     └───┬───┘            └───┬───┘  │
          │─────────┴─────────┐    ┌─────┴──────┤ 
          │                   │    │            │
          │  ┌─────────────┐  │    │            │  ┌─────────────┐
          │  │             │  │    │            │  │             │
          │  │ PIC16F18313 │  │    │            │  │ DFPlayer Pro│
          │  │             │  │    │            │  │             │
          │  │             │  │    │            │  │             │
          └──│1-VDD   VSS-8│──┴────│            └──┤VIN          │
             │             │       │               │             │
             │2-RA5   RA0-7│────┐  │───────────────┤GND          │
             │             │    │  │               │             │            +
             │3-RA4   RA1-6│──┐ └─────────[1kΩ]────┤RX       SPK-├────────┤ Speaker
             │             │  │    │               │             │            - 
             │4-RA3   RA2-5│───────────────────────┤TX       SPK+├────────┤ Speaker
             │             │  │    │               │             │
             └─────────────┘  │    │               └─────────────┘              
                              │    │        
             ┌─────────────┐  │    │        
             │             │  │    │        
             │   FS-iA6B   │  │    │        
             │             │  │    │        
             │VCC       RX ├──┘    │
             │             │       │ 
             │GND          │       │
             │             │       │
             └─────────────┘       │
                                   │
                                 ──┴──
                                  GND

Signal Connections:
- RA0 (PIC TX) ──[1kΩ]──► RX (DFPlayer)     : Commands at 9600 baud
- RA1 (PIC RX) ◄────────── OUT (FS-iA6B)    : i-Bus data at 115200 baud  
- RA2 (PIC GPIO) ◄──────── TX (DFPlayer)    : Responses at 9600 baud

Power Supply Decoupling:
- C1: 10µF 25V across PIC VDD-VSS
- C2: 22µF 25V across DFPlayer VCC-GND
```

## Detailed Pin Assignments

### PIC16F18313 Pin Functions
| Pin | Name | Function | Connection |
|-----|------|----------|------------|
| 1   | VDD  | Power    | +5V |
| 2   | RA5  | NC       | Not Connected |
| 3   | RA4  | NC       | Not Connected |
| 4   | RA3  | NC       | Not Connected |
| 5   | RA2  | Digital Input | DFPlayer Pro TX (direct connection) |
| 6   | RA1  | UART RX  | FS-iA6B i-Bus signal |
| 7   | RA0  | UART TX  | DFPlayer Pro RX (via 1kΩ resistor) |
| 8   | VSS  | Ground   | Common GND |

### DFPlayer Pro Connections
| DFPlayer Pin | Function | Connection |
|--------------|----------|------------|
| VCC | Power (+5V) | +5V Supply |
| GND | Ground | Common GND |
| RX  | Serial Input | PIC RA0 (UART TX) via 1kΩ resistor |
| TX  | Serial Output | PIC RA2 (Software UART RX) direct connection |
| SPK+ | Speaker Positive | Speaker/Audio Output |
| SPK- | Speaker Negative | Speaker/Audio Output |

### FS-iA6B Receiver Connections
| Receiver Pin | Function | Connection |
|--------------|----------|------------|
| VCC | Power (+5V) | +5V Supply |
| GND | Ground | Common GND |
| i-Bus | Serial Data | PIC RA1 (UART RX) |

### Power Supply Decoupling Capacitors
| Component | Value | Voltage Rating | Connection | Purpose |
|-----------|--------|----------------|------------|---------|
| C1 | 10µF | 25V | PIC VDD to VSS | Power supply filtering and decoupling |
| C2 | 22µF | 25V | DFPlayer VCC to GND | Power supply filtering and decoupling |

### Protection Components
| Component | Value | Connection | Purpose |
|-----------|--------|------------|---------|
| R1 | 1kΩ | PIC RA0 to DFPlayer RX | Current limiting and voltage protection |

## Communication Protocols

### UART Configuration (PIC ↔ FS-iA6B)
- **Baud Rate**: 115200
- **Data**: 8-bit
- **Parity**: None
- **Stop Bits**: 1
- **Direction**: FS-iA6B → PIC RA1 (RX only)

### UART Configuration (PIC ↔ DFPlayer Pro)
- **Baud Rate**: 9600 (commands) / 9600 (responses)
- **Data**: 8-bit
- **Parity**: None
- **Stop Bits**: 1
- **TX Direction**: PIC RA0 → DFPlayer (Hardware UART via 1kΩ resistor)
- **RX Direction**: DFPlayer → PIC RA2 (Software UART direct connection)

## Power Requirements
- **Supply Voltage**: 5V DC
- **Total Current**: ~200mA (estimated)
  - PIC16F18313: ~20mA
  - DFPlayer Pro: ~100mA
  - FS-iA6B: ~50mA

## Notes
1. **Protection Resistor**: 
   - 1kΩ resistor between PIC RA0 and DFPlayer RX protects both devices from voltage differences
2. **Decoupling Capacitors**:
   - 10µF capacitor on PIC power pins provides stable power supply filtering
   - 22µF capacitor on DFPlayer power pins handles audio processing power demands
   - Both capacitors should be placed as close as possible to their respective IC power pins
3. **Software UART**: Implemented on RA2 for reading DFPlayer responses at 9600 baud
4. **Hardware UART**: Used for both FS-iA6B reception (115200) and DFPlayer transmission (9600)
5. **i-Bus Protocol**: 32-byte packets from receiver processed via interrupt-driven ring buffer
6. **AT Commands**: Sent to DFPlayer Pro for audio control and file management

## Programming Connections (ICSP)
| ICSP Pin | PIC Pin | Function |
|----------|---------|----------|
| 1 | - | VPP (not used on PIC16F18313) |
| 2 | 8 | VSS (Ground) |
| 3 | 1 | VDD (+5V) |
| 4 | 7 | PGD (RA0/ICSPDAT) |
| 5 | 6 | PGC (RA1/ICSPCLK) |
