# SASM Compiler
This directory contains source code of the SASM compiler - compiler for a low-level programming language targeting the Stark processors.
Despite its low-level nature, SASM provides some features commonly found in high-level languages, such as scopes and aliases.

### Features:
- Targeting only Stark1 architecture
- Compile-time translation, that helps write more readable code
- Automatic source map generation for easy debugging in the emulator
- Comments
- Scoping
- Aliases

### Example
Here's how an example memcpy function implementation might look like in SASM:

```asm
set r0, 0x1000 as source_address
set r1, 0x1400 as destination_address
set r2, 32 as remaining_bytes

loop {
    # copy 1 byte from source to destination
    set [destination_address], [source_address]
    
    # move to the next byte
    inc source_address
    inc destination_address
    
    # check if there are more bytes to copy, if yes, then loop 
    dec remaining_bytes
    cmp remaining_bytes, 0
    jne loop
}

# stop execution after we're done
# this is important, because otherwise the CPU would just continue
# reading instructions from memory blocks with who knows what data
hlt
```

As you can see, thanks to high-level features, it's very easy to read and understand the code.

### Basic syntax
SASM's syntax is fairly simple - each operation is separated by a new line and has a following format:
```
operation operand1, operand2, ...
```

For example:
```asm
set r0, 123
jmp 0x1000
add r0, r1, r2
hlt
```

An operation can have any number of operands, including none.

You can start comments with a `#` character.

If you want to reference an address stored inside a register, then you need to wrap the name of this register using `[]`.
For example:
```asm
# set register's value to 0x1000
set r0, 0x1000

# put 1234 at address stored in the register (in this case, 0x1000)
set [r0], 1234
```

### Compile-time opcode translation
You might have noticed, that the instructions in the example code don't really match up with the opcodes, that the CPU expects - this is because the CPU
expects to have simple, ready-to-execute operations fed to it, and those are not always that readable.

SASM's compiler abstracts away the need to think about what opcode the CPU expect exactly and lets you write simple code, that
it then translates into proper opcodes depending on the operands.
For example, the `set` instruction has many variants: it can use 8-bit, 16-bit or 32-bit values, registers, addresses etc.
Each variant has a special opcode assigned to it, remembering all of them would be a waste of time, and would also make the code
much less readable.

### Optimization
Right now, this compiler does not perform any kind of optimizations to the code you write.

### Scopes, aliases
#### Scopes
In contrary to classic assembly languages, SASM code can be scoped and include aliases for registers and addreses to make the code more readable.
Scope is a group of instructions, it can contain any number of aliases and sub-scopes inside:

```asm
first_scope {
    # ...
}

second_scope {
    # ...
}
```

__Note:__ Scopes do fall-through! For example, if you put some instructions in the `first_scope`, after they are done, operations from `second_scope` would be executed.
You can prevent fall-through by using `jmp`, `hlt` or `ret` instructions.

#### Aliases
Aliases are very useful feature that will make you code much more readable.
See for yourself, here's how our example `memcpy` function would look like without aliases:

```asm
set r0, 0x1000
set r1, 0x1400
set r2, 32

loop {
    set [r1], [r0]
    inc r0
    inc r1
    dec r2
    cmp r2, 0
    jne loop
}

hlt
```

You can define an alias by adding `as your_name` at the end of `set` and `pop` instructions.
There can only be one alias of given name in a scope.

### Source maps
Stark 1 emulator can use source maps generated by the compiler to highlight instructions that are about to be executed.
Source maps are generated during compilation and are stored in the same directory as the output file.

Inside a source map you can find a mapping of which line maps to which opcode in the output file:
```asm
offset in file|line number
0|1
5|2
```

This means, that instruction starting at offset #0 is located in line #1, and instruction starting at offset #5 is located in line #2