## Description

A demonstration of the windowed features of the watchdog timer.

When the application begins, it initializes and starts the watchdog timer.  The application then begins to reset the watchdog within the allowed window.  Use SW2 and SW3 on the evaluation kit to control if and when the application attempts to reset the timer.

- Push SW2 to trigger a "too-late" watchdog reset. This will stop resetting the watchdog timer until it generates the "too-late" interrupt.  After that it will reset the watchdog timer only once, allowing it to pass the reset timeout period.
- Push SW3 to reset the watchdog timer in the "too-early" period.

## Required Connections

-   Connect a USB cable between the PC and the CN1 (USB/PWR) connector.
-   Connect pins 1 and 2 (P0_1) of the JH1 (UART 0 EN) header.
-   Open an terminal application on the PC and connect to the EV kit's console UART at 115200, 8-N-1.

## Expected Output

The Console UART of the device will output these messages:

```
************** Watchdog Timer Demo ****************
SW2: Push SW2 to trigger a "too-late" watchdog reset. This will stop resetting
     the watchdog timer until it generates the "too-late" interrupt.  After that
     it will reset the watchdog timer only once, allowing it to pass the reset
     timeout period.

SW3: Push SW3 to reset the watchdog timer in the "too-early" period.
```

