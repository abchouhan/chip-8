#pragma once
#include <array>
#include <cstdint>
#include <fstream>
#include <curses.h>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include "../include/Chip8Config.hpp"
#include "../include/Cpu.hpp"
#include "../include/Utils.hpp"

class Chip8 {
    private:
        // The default font sprite data stored in binary format
        std::array<uint8_t, 0x50> static constexpr defaultFont = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

        Chip8Config const config;
        bool running;
        size_t romSize;                                         // Size of ROM given by user
        std::array<uint8_t, RAM_SIZE> ram;                      // Chip-8 RAM
        std::array<bool, SCREEN_SIZE_X*SCREEN_SIZE_Y> screen;   // Display screen as boolean pixels

        Cpu cpu;

        // Input mappings hard-coded as:
        //  Keypad               Keyboard
        // ╔═══╦═══╦═══╦═══╗    ╔═══╦═══╦═══╦═══╗
        // ║ 1 ║ 2 ║ 3 ║ C ║    ║ 1 ║ 2 ║ 3 ║ 4 ║
        // ╠═══╬═══╬═══╬═══╣    ╠═══╬═══╬═══╬═══╣
        // ║ 4 ║ 5 ║ 6 ║ D ║    ║ Q ║ W ║ E ║ R ║
        // ╠═══╬═══╬═══╬═══╣ -> ╠═══╬═══╬═══╬═══╣
        // ║ 7 ║ 8 ║ 9 ║ E ║    ║ A ║ S ║ D ║ F ║
        // ╠═══╬═══╬═══╬═══╣    ╠═══╬═══╬═══╬═══╣
        // ║ A ║ 0 ║ B ║ F ║    ║ Z ║ X ║ C ║ V ║
        // ╚═══╩═══╩═══╩═══╝    ╚═══╩═══╩═══╩═══╝
        // TODO add audio
        void handleSdlInput(SDL_Event &e);
        void handleNcursesInput();
        void startSDL();
        void startTerminal();

    public:
        Chip8(Chip8Config const config);
        Chip8(Chip8 const &chip8);
        ~Chip8();
        friend std::ostream &operator<<(std::ostream &out, Chip8 const &chip8);

        // Load font file into Chip-8 RAM given a path
        bool loadFont(char const *const fontPath);
        // Load ROM file into Chip-8 RAM given a path
        bool loadRom (char const *const romPath);
        void start();
};
