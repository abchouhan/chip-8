#include "include/Chip8.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
		std::cerr << "An input file is required." << std::endl;
		std::cerr << "Use --help for a list of all options." << std::endl;
		return EXIT_FAILURE;
	}

	bool terminalMode = false;
	uint16_t clockSpeed = 900;
	uint16_t refreshRate = 60;
	bool fontSpecified = false;
	bool romSpecified = false;
	char *fontPath;
	char *romPath;

	for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0) {
            std::cout << "Usage: chip-8 [OPTIONS] [INPUT FILE]\n\nwith options:\n"
			"-t, --terminal-mode\nUse this option if the emulated display is to be output in the terminal.\n"
			"-f, --font PATH\nPath to custom font file (max 80 bytes).\n"
			"-c, --clock-speed VALUE\nNumber of instructions the CPU executes per second (Hz). Default is 900 Hz.\n"
			"-r, --refresh-rate VALUE\nHow often the display is updated in Hz. Default is 60 Hz." << std::endl;
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--terminal-mode") == 0) {
			terminalMode = true;
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--font") == 0) {
			if (i+1 < argc) {
				fontPath = argv[++i];
				fontSpecified = true;
			} else {
				std::cerr << "--font option requires one argument." << std::endl;
                return EXIT_FAILURE;
			}
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--clock-speed") == 0) {
            if (i+1 < argc) {
				try {
					int c = std::stoi(argv[++i]);
					if (c < 0) throw 1;
					clockSpeed = c;
				} catch (...) {
					std::cerr << "--clock-speed option must be a positive number." << std::endl;
					return EXIT_FAILURE;
				}
            } else {
                std::cerr << "--clock-speed option requires one argument." << std::endl;
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--refresh-rate") == 0) {
            if (i+1 < argc) {
				try {
					int r = std::stoi(argv[++i]);
					if (r < 0) throw 1;
					refreshRate = r;
				} catch (...) {
					std::cerr << "--refresh-rate option must be a positive number." << std::endl;
					return EXIT_FAILURE;
				}
            } else {
                std::cerr << "--refresh-rate option requires one argument." << std::endl;
                return EXIT_FAILURE;
            }
        } else {
            romPath = argv[i];
			romSpecified = true;
        }
	}

	Chip8Config config = {
		.terminalMode = terminalMode,
		.clockSpeed = clockSpeed,
		.refreshRate = refreshRate,
		.romStartOffset = 0x0200,	// Conventional value
		.fontStartOffset = 0x0050	// Conventional value
	};
	Chip8 chip8 = Chip8(config);

	if (fontSpecified && !chip8.loadFont(fontPath)) std::cerr << "Font was not loaded! Continuing with default font." << std::endl;

	if (romSpecified && chip8.loadRom(romPath)) chip8.start();
	else return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
