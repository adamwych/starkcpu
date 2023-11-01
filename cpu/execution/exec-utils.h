#pragma once
#include <stdbool.h>

#define DEFINE_OP(op) opcode_handlers_map_set(executor->handlers_map, op, cpu_execute_##op);
#define MAKE_OP_HANDLER(op) void cpu_execute_##op(starkcpu_t *cpu)

/* Checks whether given address can be written to by a program. */
#define is_address_writable(address) \
    (address > CPU_RESERVED_MEMORY_SIZE && address < cpu->memsize)

/* Checks whether given address can be read from by a program. */
#define is_address_readable(address) \
    (address < cpu->memsize)

/* Makes sure that given address can be written to. Panics and shuts down if it can not. */
#define assert_address_writable(address) \
    if (!is_address_writable(address)) \
        cpu_panic(cpu, "address 0x%02x is not writable", address);

/* Makes sure that given address can be read from. Panics and shuts down if it can not. */
#define assert_address_readable(address) \
    if (!is_address_readable(address)) \
        cpu_panic(cpu, "address 0x%02x is not readable", address);

/* Makes sure that given address can be jumped to. Panics and shuts down if it can not. */
#define assert_address_jmpable(address) \
    if (!is_address_writable(address)) \
        cpu_panic(cpu, "can not jump to protected address 0x%02x", address);

/* Makes sure that a register with given index exists. Panics and shuts down if it does not. */
#define assert_register_exists(index) \
    if (index < 0 || index > 7) \
        cpu_panic(cpu, "register %d does not exist", index);