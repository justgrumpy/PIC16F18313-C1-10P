### Necessary components

*   **7.4V LiPo Battery:** This will power your entire system. A 2S LiPo battery is a good choice.
*   **7.4V High Voltage (HV) Servos:** If your servos are already rated for 7.4V, you can skip the second regulator.
*   **Switching Voltage Regulator (UBEC):** A BEC (Battery Eliminator Circuit) is a type of switching voltage regulator commonly used in RC applications. You need one that can convert 7.4V (up to 8.4V when fully charged) to a stable 5V output.
    *   **Capacity:** Ensure the UBEC has enough current capacity for the Flysky receiver, PIC, and DFPlayer Pro. These are typically low-current devices, so a UBEC rated for at least 3A should be sufficient.
*   **Wiring:** Use appropriate gauge wire for connecting the battery to the regulator and components.

### Wiring instructions
1.  **Connect the Battery to the Regulator:** Connect the positive ($+$) and negative ($-$) terminals of the 7.4V battery to the input of the switching voltage regulator.
2.  **Connect Servos:** Connect the positive ($+$) and negative ($-$) terminals of the battery directly to the power inputs of your HV servos. All components must share a common ground.
3.  **Connect 5V Components to the Regulator Output:**
    *   Connect the output of the voltage regulator to the power inputs of the Flysky receiver, the PIC16F18313, and the DFPlayer Pro.
    *   **Ground:** Ensure all components share a common ground by connecting all their ground pins together.
4.  **Connect Signal and Data Lines:**
    *   **Flysky to PIC:** Connect the receiver's PWM or i-BUS output channels to the PIC's input pins.
    *   **PIC to DFPlayer Pro:** Connect the PIC's UART pins to the DFPlayer Pro's RX/TX pins.
    *   **PIC to Servos:** Connect the PIC's output pins to the signal inputs of the servos.

### Important safety considerations
*   **Check servo rating:** Before connecting the servos, double-check that they are rated for high voltage operation. Powering a standard servo with 7.4V will cause it to overheat and fail.
*   **Common Ground:** To prevent issues, ensure all components share a single common ground connection.
*   **Current Draw:** Verify that the chosen UBEC can handle the total current draw of your 5V components. While the PIC and DFPlayer are low power, they add to the load.
*   **Insulate Connections:** Use heat shrink tubing or electrical tape to insulate all connections and prevent short circuits.
Amazon.com: FEETECH 35KG Servo Motor 7.4V High Voltage ...
Buy FEETECH 35KG Servo Motor 7.4V High Voltage Waterproof High Torque RC Servo Full Metal Gear Digital Servo, Aluminum Case, Control Angle 180° for 1/8 1/10 RC ...
favicon
Amazon.com

DFPlayer Pro - A Mini MP3 Player for Arduino WiKi - DFRobot
Specification * Power Supply: 3.3V~5V. * Operating Current: >20mA. * Storage: 128MB. * Audio Format: MP3, WAV, WMA, FLAC, AAC, APE. * Communication: UART. * Ope...
favicon
DFRobot

How to Use FS-iA6B : Examples, Pinouts, and Specs
The FS-iA6B is a 6-channel receiver manufactured by FLYSKY, designed for use in remote control systems, particularly in model aircraft, drones, and other RC veh...
favicon
Cirkit Designer

