## Description

Demonstrates the use of the ADC by continuously monitoring ADC input channel 0.  Vary the voltage on the AIN0 input (0 to 0.9V) to observe different readings from the ADC.  

High and low limits are set arbitrarily to demonstrate the detection of overvoltage and undervoltage conditions respectively.  If the ADC reading exceeds 0x300, the example will report that the high limit has been reached.  If the ADC reading falls below 0x25, the example will report the low limit has been reached.

Any reading that exceeds the full-scale value of the ADC will have an '*' appended to the value.

## Required Connections

-   Connect a USB cable between the PC and the CN1 (USB/PWR) connector.
-   Connect pins 1 and 2 (P0_1) of the JH1 (UART 0 EN) header.
-   Open an terminal application on the PC and connect to the EV kit's console UART at 115200, 8-N-1.
-   Apply an input voltage between 0 and 0.9V to pin 1 of the JH3 (ADC IN) header.

## Expected Output

The Console UART of the device will output these messages:

```
ADC Example
0: 0x0001


0: 0x003f


0: 0x01ad


0: 0x028d


0: 0x0289


0: 0x006a
```
