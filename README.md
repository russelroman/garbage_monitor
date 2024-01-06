# Garbage Monitor

Garbage Monitor will get the current fill level of the garbage bin and will transmit the
fill level percentage to Thingspeak

The project uses stm32 and M95 GSM module to transmit data to Thingspeak

## Building the project

There are two ways to build the project:
1. Build the project using custom Makefile
2. Build the project using STM32Cube IDE

### Build the project using custom Makefile

*make all - will build the binary of the project\n
*make flash - will flash the binary to STM32 target device\n
*make clean - will delete the build artifacts\n

### Build the project using STM32Cube IDE

1. Open the stm32 project file located in Projects folder
2. Build using stm32cube ide
