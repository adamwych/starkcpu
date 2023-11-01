# Stark 1 emulator
This directory contains the source code of the emulator, that simulates behaviour of a made-up CPU called Stark 1.
Emulator consists of two parts:
- Core emulator - all code, that takes care of simulating the CPU.
- UI - visualizes current state of the CPU in terminal using ncurses library.

### Dependencies
- cmake >= 3.17
- C compiler with support for at least C11 standard
- ncurses - library used to easily show nice things in the terminal. If you're on Linux or macOS, then this library should already be present in your system. If you're on Windows, you need to download PDCurses, build it and put it in the root directory.

### Building
```
mkdir build
cmake -GNinja ..
ninja
```

### Running
```
./emulator <input file path>
```

### How does it work?
Stark CPU is a 32-bit, kinda RISC, kinda CISC processor. It has eight 32-bit general purpose registers named R0-R7, implements opcodes to operate directly on the memory and on the registers.

This emulator focuses mainly on reading, decoding and executing instructions stored in a raw binary file.
Instructions are read one-by-one and executed by the software using host's CPU, but their result is sometimes altered to match the result of a Stark CPU.
It does *not* simulate components like an ALU in the software.

Emulator reserves 256 bytes of memory for internal use by the CPU, it contains things like model, version, register values, stack etc.
This block of memory starts at address 0x00000000 and ends at 0x00000100. Attempting to write to this memory region will result in CPU panicking and immediately stopping execution.

Startup process is fairly simple:
- Allocate required block of memory.
- Initialize internal memory.
- Load provided binary file at address 0x00000100.
- Set instruction pointer to address 0x00000100 and start execution loop.

Execution loop is as follows:
- Read instruction from address pointed to by the instruction pointer.
- Decode and execute instruction, change register values, set flags etc.
- Read next instruction...

For complete list of opcodes you can take a look [here](OPCODES.md).