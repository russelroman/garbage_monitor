# Garbage Monitor

Garbage Monitor will get the current fill level of the garbage bin and will transmit the
fill level percentage to Thingspeak. The device will notify the user if the fill level
reaches to a certain level.

The project uses stm32 and M95 GSM module to transmit data to Thingspeak

## Building the project

There are two ways to build the project:
1. Build the project using custom Makefile
2. Build the project using STM32Cube IDE

### Build the project using custom Makefile

make all - will build the binary of the project<br>
make flash - will flash the binary to STM32 target device<br>
make clean - will delete the build artifacts<br>

### Build the project using STM32Cube IDE

1. Open the stm32 project file located in Projects folder
2. Build using stm32cube ide

## Build the tests

make -f MakeFile_test.mk
