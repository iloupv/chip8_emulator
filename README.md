# CHIP-8 Emulator
A simple CHIP-8 emulator written in C++.
## Installation
Make sure you have the SDL libraries installed.
```Bash
cd bin 
cmake .. 
make 
```
A file named chip8 should have been generated.
## Usage
To execute the binary file, type this:
```Bash
./chip8 <Cycles per Second> <ROM Filename>
```
- Pressing F1 will reset the emulator.
- Pressing F2 will enable the debugger.
- Pressing F3 will save the current state of the emulator.
- Pressing F4 will load the saved state of the emulator.
- Pressing the spacebar will stop the current cycle. In this state, user can press the (+) key to execute the next CPU cycles step by step.
