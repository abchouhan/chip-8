# chip-8
A simple [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) emulator/interpreter that can be played on the terminal!

![Chip8TerminalTetrisDemo](https://github.com/user-attachments/assets/4f4d599e-700d-41de-a5b2-e4bd3f4779a6)
![Chip8TerminalHiddenDemo](https://github.com/user-attachments/assets/8c301078-47cf-453f-b1a4-5ab72144479f)

## Usage
```
chip-8 [OPTIONS] [INPUT FILE]

with options:
-t, --terminal-mode
Use this option if the emulated display is to be output in the terminal.
-f, --font PATH
Path to custom font file (max 80 bytes).
-c, --clock-speed VALUE
Number of instructions the CPU executes per second (Hz). Default is 900 Hz.
-r, --refresh-rate VALUE
How often the display is updated in Hz. Default is 60 Hz.
```
### Input
| CHIP-8 Keypad | Keyboard |
| ------------- | -------- |
| <kbd>1</kbd><kbd>2</kbd><kbd>3</kbd><kbd>C</kbd><br><kbd>4</kbd><kbd>5</kbd><kbd>6</kbd><kbd>D</kbd><br><kbd>7</kbd><kbd>8</kbd><kbd>9</kbd><kbd>E</kbd><br><kbd>A</kbd><kbd>0</kbd><kbd>B</kbd><kbd>F</kbd><br> | <kbd>1</kbd><kbd>2</kbd><kbd>3</kbd><kbd>4</kbd><br><kbd>Q</kbd><kbd>W</kbd><kbd>E</kbd><kbd>R</kbd><br><kbd>A</kbd><kbd>S</kbd><kbd>D</kbd><kbd>F</kbd><br><kbd>Z</kbd><kbd>X</kbd><kbd>C</kbd><kbd>V</kbd><br> |

## TODO
- Add audio for sound timer
- Implement SUPER-CHIP instructions and allow user to toggle between them
