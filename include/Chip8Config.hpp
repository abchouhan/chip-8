#pragma once
#include <cstdint>

#define RAM_SIZE 4096           // Size of CHIP-8 memory in bytes
#define RESERVED_BYTES 352      // The number of bytes at the end of memory reserved for variables and display refresh
#define STACK_SIZE 16           // Number of 2-byte values the call stack can store
#define FONT_SIZE 0x50          // The exact number of bytes a given font file should be
#define SCREEN_SIZE_X 64        // Number of horizontal pixels in the screen display
#define SCREEN_SIZE_Y 32        // Number of vertical pixels in the screen display
#define SCREEN_SCALE_FACTOR 12  // Resolution multiplier for the display (only for SDL)

struct Chip8Config {
    bool terminalMode;          // Determines whether to render using SDL or ncurses
    uint16_t clockSpeed;        // Number of instructions the CPU executes per second (Hz)
    uint16_t refreshRate;       // How often the display is updated in Hz
    uint16_t romStartOffset;    // Offset in memory where the given ROM is stored
    uint16_t fontStartOffset;   // Offset in memory where the font data is stored
};
