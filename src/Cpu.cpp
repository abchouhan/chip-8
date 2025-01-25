#include "../include/Cpu.hpp"

Cpu::Cpu(std::array<uint8_t, 4096> &ram, uint16_t romStartOffset, uint16_t fontStartOffset, bool *const screen) :
    pc(romStartOffset),
    reg{0},
    regI(0x0000),
    delayTimer(0x00),
    soundTimer(0x00),
    fontStartOffset(fontStartOffset),
    ram(ram),
    stack{0},
    stackPointer(0),
    opcode(0x0000),
    screen(screen),
    keys{false},
    lastPressedKey(0x10),
    lastReleasedKey(0x10),
    autoReleaseKey(false) {
        TRACE("[CPU: Creating new Cpu " << this << "]");
        srand(time(0));
    }
Cpu::Cpu(Cpu const &cpu) :
    pc(cpu.pc),
    reg(cpu.reg),
    regI(cpu.regI),
    delayTimer(cpu.delayTimer),
    soundTimer(cpu.soundTimer),
    fontStartOffset(cpu.fontStartOffset),
    ram(cpu.ram),
    stack(cpu.stack),
    stackPointer(cpu.stackPointer),
    opcode(cpu.opcode),
    screen(cpu.screen),
    keys(cpu.keys),
    lastPressedKey(cpu.lastPressedKey),
    lastReleasedKey(cpu.lastReleasedKey),
    autoReleaseKey(cpu.autoReleaseKey) {
        TRACE("[CPU: Copy constructor for Cpu " << this << ", copied from " << &cpu << "]");
    }
Cpu::~Cpu() { TRACE("[CPU: deleting Cpu " << this << "]"); }

std::ostream &operator<<(std::ostream &out, Cpu const &cpu) {
    out << "CPU:\n\tProgram counter: " << stringHex(cpu.pc, 4).rdbuf() << "\n\tRegister I: " << stringHex(cpu.regI, 4).rdbuf()
        << "\n\tRegisters:\n\t\t  V0   V1   V2   V3   V4   V5   V6   V7   V8   V9   VA   VB   VC   VD   VE   VF\n\t\t";
    for (int i = 0; i < cpu.reg.size(); i++) {
        out << stringHex(cpu.reg[i], 2).rdbuf();
        if (i != cpu.reg.size()-1) out << " ";
    }
    out << "\n\tStack:\n\t";
    for (int i = 0; i < cpu.stack.size(); i++) {
        if (i == cpu.stackPointer) out << YELLOW << stringHex(cpu.stack[i], 4, false).rdbuf() << RESET;
        else out << stringHex(cpu.stack[i], 4, false).rdbuf();
        if (i != cpu.stack.size()-1) out << " ";
    }
    return out;
}

bool Cpu::stackPush(uint16_t value) {
    if (stackPointer >= stack.size()-1) {
        std::cerr << RED << "Error! Stack overflow!" << RESET << std::endl;
        return false;
    }
    stack[stackPointer] = pc;
    stackPointer++;
    return true;
}
std::optional<uint16_t> Cpu::stackPop() {
    if (stackPointer <= 0) {
        std::cerr << RED << "Error! Stack underflow!" << RESET << std::endl;
        return std::nullopt;
    }
    stackPointer--;
    return stack[stackPointer];
}

void Cpu::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

void Cpu::pressKey(uint8_t key) {
    if (key > 0xF) return;
    keys[key] = true;
    if (autoReleaseKey) lastPressedKey = key;
}
void Cpu::releaseKey(uint8_t key) {
    if (key > 0xF) return;
    keys[key] = false;
    lastReleasedKey = key;
}
void Cpu::setAutoReleaseKey(bool autoReleaseKey) {
    this->autoReleaseKey = autoReleaseKey;
}

void Cpu::clock() {
    opcode = (ram[pc] << 8) | ram[pc+1];
    pc += 2;

    switch (opcode >> 12) {
        case 0x0: opcode0(); break;
        case 0x1: opcode1(); break;
        case 0x2: opcode2(); break;
        case 0x3: opcode3(); break;
        case 0x4: opcode4(); break;
        case 0x5: opcode5(); break;
        case 0x6: opcode6(); break;
        case 0x7: opcode7(); break;
        case 0x8: opcode8(); break;
        case 0x9: opcode9(); break;
        case 0xA: opcodeA(); break;
        case 0xB: opcodeB(); break;
        case 0xC: opcodeC(); break;
        case 0xD: opcodeD(); break;
        case 0xE: opcodeE(); break;
        case 0xF: opcodeF(); break;
        default: break;
    }
}

void Cpu::opcode0() {
    if (opcode == 0x00E0) {         // Clear screen instruction
        for (int i = 0; i < SCREEN_SIZE_X*SCREEN_SIZE_Y; i++) {
            screen[i] = false;
        }
    } else if (opcode == 0x00EE) {  // Return from a subroutine (function call)
        std::optional<uint16_t> popVal = stackPop();
        if (popVal) pc = popVal.value();
    }
    // 0NNN instructions are not necessary to implement, so they are not processed
}
// 1NNN: Jump to address NNN
void Cpu::opcode1() { pc = NNN(opcode); }
// 2NNN: Call a subroutine (function) at address NNN
void Cpu::opcode2() {
    if (!stackPush(pc)) return;
    opcode1();
}
// 3XNN: If the value of register VX is NN, skip the following instruction
void Cpu::opcode3() {
    if (reg[X(opcode)] == NN(opcode)) { pc += 2; }
}
// 4XNN: If the value of register VX is not NN, skip the following instruction
void Cpu::opcode4() {
    if (reg[X(opcode)] != NN(opcode)) { pc += 2; }
}
// 5XY0: If the value of register VX is equal to the value of register VY, skip the following instruction
void Cpu::opcode5() {
    if (reg[X(opcode)] == reg[Y(opcode)]) { pc += 2; }
}
// 6XNN: Store NN in register VX
void Cpu::opcode6() { reg[X(opcode)]  = NN(opcode); }
// 7XNN: Add NN to the value of register VX
void Cpu::opcode7() { reg[X(opcode)] += NN(opcode); }
// 8XYN
void Cpu::opcode8() {
    switch (N(opcode)) {
        case 0x0: reg[X(opcode)]  = reg[Y(opcode)]; break;  // 8XY0: Store the value of register VY in register VX
        case 0x1: reg[X(opcode)] |= reg[Y(opcode)]; break;  // 8XY1: Store VX bitwise OR'd with VY in register VX
        case 0x2: reg[X(opcode)] &= reg[Y(opcode)]; break;  // 8XY2: Store VX bitwise AND'd with VY in register VX
        case 0x3: reg[X(opcode)] ^= reg[Y(opcode)]; break;  // 8XY3: Store VX bitwise XOR'd with VY in register VX
        // 8XY4: Add the value of register VY to register VX, setting register VF to 1 if a carry occurs, 0 otherwise
        case 0x4: {
            uint8_t regX = reg[X(opcode)];
            uint8_t regY = reg[Y(opcode)];
            if ((uint16_t)(regX+regY) > 0xFF) reg[0xF] = 0x01;
            else reg[0xF] = 0x00;
            reg[X(opcode)] = regX+regY;
            break;
        }
        // 8XY5: Subtract the value of register VY from register VX, setting register VF to 0 if a borrow occurs, 1 otherwise
        case 0x5: {
            uint8_t regX = reg[X(opcode)];
            uint8_t regY = reg[Y(opcode)];
            if (regX > regY) reg[0xF] = 0x01;
            else reg[0xF] = 0x00;
            reg[X(opcode)] = regX-regY;
            break;
        }
        // 8XY6: Set the value of register VX to the value of VY shifted right by one bit, setting VF to the rightmost bit of VY
        case 0x6: // TODO Super-CHIP
            reg[0xF] = reg[Y(opcode)] & 0x01;
            reg[X(opcode)] = reg[Y(opcode)] >> 1;
            break;
        // 8XY7: Set the value of register VX to VY minus VX, setting register VF to 0 if a borrow occurs, 1 otherwise
        case 0x7: {
            uint8_t regX = reg[X(opcode)];
            uint8_t regY = reg[Y(opcode)];
            if (regY > regX) reg[0xF] = 0x01;
            else reg[0xF] = 0x00;
            reg[X(opcode)] = regY-regX;
            break;
        }
        // 8XYE: Set the value of register VX to the value of VY shifted left by one bit, setting VF to the leftmost bit of VY
        case 0xE: // TODO Super-CHIP
            reg[0xF] = ((reg[Y(opcode)] & 0x80) >> 7) & 0x01;
            reg[X(opcode)] = reg[Y(opcode)] << 1;
            break;
        default: break;
    }
}
// 9XY0: If the value of register VX is not equal to the value of register VY, skip the following instruction
void Cpu::opcode9() {
    if (reg[X(opcode)] != reg[Y(opcode)]) { pc += 2; }
}
void Cpu::opcodeA() { regI = NNN(opcode); }         // ANNN: Store NNN in register I
void Cpu::opcodeB() { pc = NNN(opcode) + reg[0]; }  // BNNN: Jump to address NNN + V0
// CXNN: Set register VX to a random value bitwise AND'd with NN
void Cpu::opcodeC() { reg[X(opcode)] = rand() & NN(opcode); }
// DXYN: Draw a sprite at screen position (VX, VY) using N bytes of sprite data stored at the offset in memory specified by register I,
// setting register VF to 1 if any pixels are turned off after drawing, 0 otherwise
void Cpu::opcodeD() {
    // The sprite position wraps around the screen, so take the modulo
    uint8_t xPos = reg[X(opcode)] % SCREEN_SIZE_X;
    uint8_t yPos = reg[Y(opcode)] % SCREEN_SIZE_Y;
    reg[0xF] = 0;
    // The sprite data offset also corresponds to the y-position;
    // the N bytes of sprite data is vertically stacked
    for (int y = 0; y < N(opcode); y++) {
        if (yPos+y >= SCREEN_SIZE_Y) break;     // The sprite itself does not wrap
        uint8_t spriteData = ram[regI+y];
        for (int x = 0; x < 8; x++) {
            if (xPos+x >= SCREEN_SIZE_X) break; // The sprite itself does not wrap
            // Each byte of sprite data is 8 horizontal pixels
            if ((spriteData >> (7-x)) & 0x01) {
                bool *pixel = screen+(yPos+y)*SCREEN_SIZE_X+xPos+x;
                if (*pixel) {
                    reg[0xF] = 1;
                    *pixel = false;
                } else *pixel = true;
            }
        }
    }
}
void Cpu::opcodeE() {
    uint8_t VX = reg[X(opcode)];
    if (VX > 0xF) return;
    uint8_t subOpCode = NN(opcode);
    if (subOpCode == 0x9E) {         // EX9E: If the value of register VX corresponds to a key that is currently pressed, skip the following instruction
        if (keys[VX]) {
            pc += 2;
            // Since the key press has been registered, release it if the flag autoReleaseKey is true
            if (autoReleaseKey) releaseKey(VX);
        }
    } else if (subOpCode == 0xA1) {  // EXA1: If the value of register VX corresponds to a key that is not currently pressed, skip the following instruction
        if (!keys[VX]) {
            pc += 2;
        } else {
            // Since the key press has been registered, release it if the flag autoReleaseKey is true
            if (autoReleaseKey) releaseKey(VX);
        }
    }
}
void Cpu::opcodeF() {
    uint8_t subOpCode = NN(opcode);
    switch (subOpCode) {
        case 0x07: reg[X(opcode)] = delayTimer; break;  // FX07: Store the current value of the delay timer in register VX
        case 0x0A:                                      // FX0A: Wait for a keypress and store the result in register VX
            if (!autoReleaseKey) {
                if (lastReleasedKey > 0xF) pc -= 2;     // Waiting is done by decrementing the program counter
                else {
                    reg[X(opcode)] = lastReleasedKey;
                    lastReleasedKey = 0x10;
                }
            } else {
                if (lastPressedKey > 0xF) pc -= 2;      // Waiting is done by decrementing the program counter
                else {
                    reg[X(opcode)] = lastPressedKey;
                    releaseKey(lastPressedKey);
                    lastPressedKey = 0x10;
                }
            }
            break;
        case 0x15: delayTimer = reg[X(opcode)]; break;              // FX15: Set the delay timer to the value of register VX
        case 0x18: soundTimer = reg[X(opcode)]; break;              // FX18: Set the sound timer to the value of register VX
        case 0x1E: regI += reg[X(opcode)]; break;                   // FX1E: Add the value of register VX to register I
        // FX29: Set register I to the memory address of font sprite data for the hexadecimal digit specified by register VX
        case 0x29: regI = fontStartOffset+reg[X(opcode)]*5; break;
        // FX33: Store the value of VX as three base-10 digits at memory addresses I, I+1, and I+2
        case 0x33: {
            uint8_t value = reg[X(opcode)];
            ram[regI] = value/100;
            ram[regI+1] = value/10%10;
            ram[regI+2] = value%100%10;
            break;
        }
        // FX55: Store the values of registers V0 to VX (inclusive) at memory addresses I, I+1, ..., I+X, then set register I to I+X+1
        case 0x55:
            for (int i = 0; i <= X(opcode); i++) {
                ram[regI+i] = reg[i];
            }
            break;
        // FX65: Set the values of registers V0 to VX (inclusive) to values at memory addresses I, I+1, ..., I+X, then set register I to I+X+1
        case 0x65:
            for (int i = 0; i <= X(opcode); i++) {
                reg[i] = ram[regI+i];
            }
            break;
        default: break;
    }
}
