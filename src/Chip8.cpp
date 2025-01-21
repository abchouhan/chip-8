#include "../include/Chip8.hpp"

Chip8::Chip8(Chip8Config const config) :
    config(config),
    running(false),
    romSize(0),
    ram{0},
    screen{false},
    cpu(Cpu(ram, config.romStartOffset, config.fontStartOffset, screen.data())) {
        TRACE("[CHIP8: Creating new Chip8 " << this << "]");
        if (config.terminalMode) cpu.setAutoReleaseKey(true);
        // Copy the default font data to CHIP-8 memory
        std::copy_n(Chip8::defaultFont.begin(), Chip8::defaultFont.size(), ram.begin()+config.fontStartOffset);
    }
Chip8::Chip8(Chip8 const &chip8) :
    config(chip8.config),
    running(chip8.running),
    romSize(chip8.romSize),
    ram(chip8.ram),
    screen(chip8.screen),
    cpu(chip8.cpu) {
    TRACE("[CHIP8: Copy constructor for Chip8 " << this << ", copied from " << &chip8 << "]");
}
Chip8::~Chip8() { TRACE("[CHIP8: deleting Chip8 " << this << "]"); }

std::ostream &operator<<(std::ostream &out, Chip8 const &chip8) {
    out << "CONFIG:\n\tROM start offset: " << stringHex(chip8.config.romStartOffset, 4).rdbuf()
        << "\n\tRAM size: " << RAM_SIZE << " B\n" << chip8.cpu;
    out << "\n\tScreen:\n";
    for (int i = 0; i < SCREEN_SIZE_X+2; i++) {
        if (i == 0) out << "┌";
        else if (i == SCREEN_SIZE_X+1) out << "┐\n";
        else out << "──";
    }
    for (int i = 0; i < SCREEN_SIZE_X*SCREEN_SIZE_Y; i++) {
        int x = i % SCREEN_SIZE_X;
        if (x == 0) out << "│";
        if (chip8.screen[i]) out << "██";
        else out << "  ";
        if (x == SCREEN_SIZE_X-1) out << "│" << "\n";
    }
    for (int i = 0; i < SCREEN_SIZE_X+2; i++) {
        if (i == 0) out << "└";
        else if (i == SCREEN_SIZE_X+1) out << "┘";
        else out << "──";
    }
    out << "\nRAM:\n";
	for (int i = 0; i < RAM_SIZE; i++) {
        // If at the beginning of a line
        if (i % 16 == 0) {
            out << "\t" << stringHex(i, 4).rdbuf() << " ";
            if (i < chip8.config.romStartOffset) out << RED;
            if (i >= RAM_SIZE-RESERVED_BYTES) out << GREEN;
        } else if (i == RAM_SIZE-RESERVED_BYTES) {
            out << GREEN;
        }

        if (chip8.cpu.getPc() == i) out << YELLOW;

        out << stringHex(chip8.ram[i], 2, false).rdbuf();

		// If at the end of a line
        if ((i+1) % 16 == 0) out << RESET << "\n";
        else {
            if (chip8.cpu.getPc()+1 == i) out << RESET << " ";
            else out << " ";
        }
	}
	out << RESET;
    return out;
}

bool Chip8::loadFont(char const *const fontPath) {
    std::ifstream file(fontPath, std::ios::binary);
    if (!file.good()) {
        std::cerr << "Error! Font file " << fontPath << " does not exist!" << std::endl;
        file.close();
        return false;
    }
    // tellg reports correct file size since it's opened in binary mode
    file.seekg(0, std::ios::end);
    size_t fontDataSize = file.tellg();
    if (fontDataSize != FONT_SIZE) {
        std::cerr << "Error! Font file is invalid, must be exactly " << FONT_SIZE << " bytes" << std::endl;
        file.close();
        return false;
    }
    file.clear();
    file.seekg(0);
    file.read(reinterpret_cast<char *>(&ram[config.fontStartOffset]), fontDataSize);
    file.close();
    return true;
}
bool Chip8::loadRom(char const *const romPath) {
    std::ifstream file(romPath, std::ios::binary);
    if (!file.good()) {
        std::cerr << "Error! ROM file " << romPath << " does not exist!" << std::endl;
        file.close();
        return false;
    }
    // tellg reports correct file size since it's opened in binary mode
    file.seekg(0, std::ios::end);
    romSize = file.tellg();
    size_t max_size = RAM_SIZE-RESERVED_BYTES-config.romStartOffset;
    if (romSize > max_size) {
        std::cerr << "Error! ROM is too large for memory, must be at most " << max_size << " bytes" << std::endl;
        file.close();
        return false;
    }
    file.clear();
    file.seekg(0);
    file.read(reinterpret_cast<char *>(&ram[config.romStartOffset]), romSize);
    file.close();
    return true;
}

void Chip8::handleSdlInput(SDL_Event &e) {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;
        else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            uint8_t key = 0x10;
            switch (e.key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                case SDL_SCANCODE_RETURN:
                case SDL_SCANCODE_RETURN2: running = false; break;
                case SDL_SCANCODE_1: key = 0x1; break;
                case SDL_SCANCODE_2: key = 0x2; break;
                case SDL_SCANCODE_3: key = 0x3; break;
                case SDL_SCANCODE_4: key = 0xC; break;
                case SDL_SCANCODE_Q: key = 0x4; break;
                case SDL_SCANCODE_W: key = 0x5; break;
                case SDL_SCANCODE_E: key = 0x6; break;
                case SDL_SCANCODE_R: key = 0xD; break;
                case SDL_SCANCODE_A: key = 0x7; break;
                case SDL_SCANCODE_S: key = 0x8; break;
                case SDL_SCANCODE_D: key = 0x9; break;
                case SDL_SCANCODE_F: key = 0xE; break;
                case SDL_SCANCODE_Z: key = 0xA; break;
                case SDL_SCANCODE_X: key = 0x0; break;
                case SDL_SCANCODE_C: key = 0xB; break;
                case SDL_SCANCODE_V: key = 0xF; break;
                default: break;
            }
            if (key > 0xF) break;
            if (e.type == SDL_KEYDOWN) cpu.pressKey(key);
            else cpu.releaseKey(key);
        }
    }
}
void Chip8::handleNcursesInput() {
    int input = getch();
    switch (input) {
        case 27: { // ESC
            int esc_input = getch();
            // If ESC key alone (not escape sequence)
            if (esc_input == ERR) running = false;
            break;
        }
        case '\n': // ENTER
            running = false;
            break;
        case ERR: break;
        case '1': cpu.pressKey(0x1); break;
        case '2': cpu.pressKey(0x2); break;
        case '3': cpu.pressKey(0x3); break;
        case '4': cpu.pressKey(0xC); break;
        case 'q': cpu.pressKey(0x4); break;
        case 'w': cpu.pressKey(0x5); break;
        case 'e': cpu.pressKey(0x6); break;
        case 'r': cpu.pressKey(0xD); break;
        case 'a': cpu.pressKey(0x7); break;
        case 's': cpu.pressKey(0x8); break;
        case 'd': cpu.pressKey(0x9); break;
        case 'f': cpu.pressKey(0xE); break;
        case 'z': cpu.pressKey(0xA); break;
        case 'x': cpu.pressKey(0x0); break;
        case 'c': cpu.pressKey(0xB); break;
        case 'v': cpu.pressKey(0xF); break;
        default: break;
    }
}

void Chip8::start() {
    if (config.terminalMode) startTerminal();
    else startSDL();
}
void Chip8::startSDL() {
    if (running) return;
    running = true;

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window *window = SDL_CreateWindow("chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_SIZE_X*SCREEN_SCALE_FACTOR, SCREEN_SIZE_Y*SCREEN_SCALE_FACTOR, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetScale(renderer, SCREEN_SCALE_FACTOR, SCREEN_SCALE_FACTOR);
    SDL_Event e;

    // Timings are in seconds
    uint64_t prev = 0;
    uint64_t now = SDL_GetPerformanceCounter();
    float performanceFrequency = (float)SDL_GetPerformanceFrequency();
    float deltaTime = 0.0;
    float clockAccumulator = 0.0f;
    float refreshAccumulator = 0.0f;
    float sixtyHzAccumulator = 0.0f;
    float const sixtyHz = 1.0f/60.0f;
    float const clockSpeed = 1.0f/config.clockSpeed;
    float const refreshRate = 1.0f/config.refreshRate;

    while (running) {
        prev = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (float)((now-prev)/performanceFrequency);
        sixtyHzAccumulator += deltaTime;
        clockAccumulator += deltaTime;
        refreshAccumulator += deltaTime;

        handleSdlInput(e);

        if (sixtyHzAccumulator >= sixtyHz) {
            sixtyHzAccumulator -= sixtyHz;
            cpu.updateTimers();
        }
        if (clockAccumulator >= clockSpeed) {
            clockAccumulator -= clockSpeed;
            cpu.clock();
        }
        if (refreshAccumulator >= refreshRate) {
            refreshAccumulator -= refreshRate;
            for (int i = 0; i < SCREEN_SIZE_X*SCREEN_SIZE_Y; i++) {
                int x = i % SCREEN_SIZE_X;
                int y = i / SCREEN_SIZE_X;
                if (screen[i]) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
            SDL_RenderPresent(renderer);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_TIMER);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    SDL_Quit();
}
void Chip8::startTerminal() {
    if (running) return;
    running = true;

    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    cbreak();
    timeout(0);

    // Timings are in seconds
    std::chrono::time_point prev = std::chrono::high_resolution_clock::now();
    std::chrono::time_point now  = prev;
    float clockAccumulator = 0.0f;
    float refreshAccumulator = 0.0f;
    float sixtyHzAccumulator = 0.0f;
    float const sixtyHz = 1.0f/60.0f;
    float const clockSpeed = 1.0f/config.clockSpeed;
    float const refreshRate = 1.0f/config.refreshRate;

    while (running) {
        prev = now;
        now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> const elapsed = now - prev;
        float deltaTime = elapsed.count();
        sixtyHzAccumulator += deltaTime;
        clockAccumulator += deltaTime;
        refreshAccumulator += deltaTime;

        handleNcursesInput();

        if (sixtyHzAccumulator >= sixtyHz) {
            sixtyHzAccumulator -= sixtyHz;
            cpu.updateTimers();
        }
        if (clockAccumulator >= clockSpeed) {
            clockAccumulator -= clockSpeed;
            cpu.clock();
        }
        if (refreshAccumulator >= refreshRate) {
            refreshAccumulator -= refreshRate;
            for (int i = 0; i < SCREEN_SIZE_X*SCREEN_SIZE_Y; i++) {
                int x = (i % SCREEN_SIZE_X)*2;
                int y = i / SCREEN_SIZE_X;
                if (screen[i]) attron(A_STANDOUT);
                else attroff(A_STANDOUT);
                mvprintw(y, x, "  ");
            }
            refresh();
        }
    }
    endwin();
}
