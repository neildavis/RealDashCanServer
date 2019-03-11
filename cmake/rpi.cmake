# CMake setup used for Builds on Raspberry Pi.

set(lib_base /usr/include)

add_definitions(-O3 -mcpu=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard)
