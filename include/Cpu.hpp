#pragma once
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <optional>
#include "../include/Chip8Config.hpp"
#include "../include/Utils.hpp"

class Cpu {
    private:
        uint16_t pc;                            // Program counter
        std::array<uint8_t, 16> reg;            // General 1-byte registers V0, V1, ..., VE, VF
        uint16_t regI;                          // 2-byte register I
        uint8_t delayTimer;                     // Delay timer, if >0, decremented at a rate of 60 Hz
        uint8_t soundTimer;                     // Sound timer, if >0, decremented at a rate of 60 Hz
        uint16_t fontStartOffset;               // Offset in memory where the font data is stored
        std::array<uint8_t, RAM_SIZE> &ram;     // CHIP-8 memory initialized in Chip8.cpp
        std::array<uint16_t, STACK_SIZE> stack; // Call stack
        uint8_t stackPointer;

        // Push a value onto the call stack
        bool stackPush(uint16_t value);
        // Pop a value from the call stack
        // If stack pointer is invalid, return nullopt
        // to indicate that something went wrong
        std::optional<uint16_t> stackPop();

        // Given a 2-byte value, return its last hexadecimal digit
        inline uint8_t    N(uint16_t value) const { return value & 0x000F; };
        // Given a 2-byte value, return its last byte
        inline uint8_t   NN(uint16_t value) const { return value & 0x00FF; };
        // Given a 2-byte value, return its last 3 hexadecimal bits
        inline uint16_t NNN(uint16_t value) const { return value & 0x0FFF; };
        // Given a 2-byte value, return the 3rd hexadecimal bit from the end
        inline uint8_t    X(uint16_t value) const { return (value >> 8) & 0x000F; };
        // Given a 2-byte value, return the 2nd hexadecimal bit from the end
        inline uint8_t    Y(uint16_t value) const { return (value >> 4) & 0x000F; };

        // Stores the current opcode pointed to by the program counter
        uint16_t opcode;
               void opcode0(); inline void opcode1(); inline void opcode2(); inline void opcode3();
        inline void opcode4(); inline void opcode5(); inline void opcode6(); inline void opcode7();
               void opcode8(); inline void opcode9(); inline void opcodeA(); inline void opcodeB();
        inline void opcodeC();        void opcodeD();        void opcodeE();        void opcodeF();

        // Screen display defined in Chip8.cpp
        bool *const screen;

        // Keypad with 16 keys 0-F
        std::array<bool, 16> keys;
        // Is set to a hex value between 0-F corresponding to a key press/release
        // A value greater than 0xF is treated as a null key
        uint8_t lastPressedKey;
        uint8_t lastReleasedKey;

        // Whether to automatically release a pressed key after it is processed
        // Is set to true when running the program in terminal mode since
        // it is not possible to detect key release events with ncurses
        bool autoReleaseKey;

    public:
        Cpu(std::array<uint8_t, RAM_SIZE> &ram, uint16_t romStartOffset, uint16_t fontStartOffset, bool *const screen);
        Cpu(Cpu const &cpu);
        ~Cpu();
        uint16_t getPc() const { return pc; };
        friend std::ostream &operator<<(std::ostream &out, Cpu const &cpu);

        // Execute one instruction
        void clock();
        // Called at a frequency of 60 Hz
        // to decrement the delay and sound timers
        void updateTimers();
        void pressKey(uint8_t key);
        void releaseKey(uint8_t key);
        void setAutoReleaseKey(bool autoReleaseKey);
};
