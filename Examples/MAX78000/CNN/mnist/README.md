## Description

This is the "Hello World" of machine learning. It uses the MNIST dataset of handwritten digits. See http://yann.lecun.com/exdb/mnist/. The input size is 28x28 pixels monochrome (i.e., 1x28x28 in CHW notation).

The code is auto-generated by the ai8x-synthesis tool and runs a known-answer
test with a pre-defined input sample. This example will be enhance with live capture and TFT
output in the near future.

## Required Connections

-   Connect a USB cable between the PC and the CN1 (USB/PWR) connector.
-   Connect pins 1 and 2 (P0_1) of the JH1 (UART 0 EN) header.
-   Open an terminal application on the PC and connect to the EV kit's console UART at 115200, 8-N-1.

## Expected Output

The Console UART of the device will output these messages:

```
*** CNN Test ***

*** PASS ***

Time for CNN: 1386 us

Classification results:
[ -29649] -> Class 0: 4.2%
[ -25548] -> Class 1: 4.2%
[ -27429] -> Class 2: 4.2%
[ -25779] -> Class 3: 4.2%
[ -25914] -> Class 4: 4.2%
[ -38948] -> Class 5: 2.1%
[ -68083] -> Class 6: 0.5%
[  39323] -> Class 7: 67.7%
[ -31039] -> Class 8: 4.2%
[ -28505] -> Class 9: 4.2%
```

