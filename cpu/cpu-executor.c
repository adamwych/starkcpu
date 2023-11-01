#include "cpu-executor.h"
#include "execution/exec-utils.h"
#include "../shared/stark1-opcodes.h"
#include <stdlib.h>

MAKE_OP_HANDLER(OP_NOP) {
    // does nothing
}

MAKE_OP_HANDLER(OP_SET_REG_IMMEDIATE8) {
    uint8_t register_index = cpu_read_program(cpu);
    uint8_t value = cpu_read_program(cpu);
    assert_register_exists(register_index);
    cpu_set_register_value(cpu, register_index, value);
}

MAKE_OP_HANDLER(OP_SET_REG_IMMEDIATE16) {
    uint8_t register_index = cpu_read_program(cpu);
    uint16_t value = cpu_read_program_int16(cpu);
    assert_register_exists(register_index);
    cpu_set_register_value(cpu, register_index, value);
}

MAKE_OP_HANDLER(OP_SET_REG_IMMEDIATE32) {
    uint8_t register_index = cpu_read_program(cpu);
    uint32_t value = cpu_read_program_int32(cpu);
    assert_register_exists(register_index);
    cpu_set_register_value(cpu, register_index, value);
}

MAKE_OP_HANDLER(OP_SET_REG_ADDR) {
    uint8_t destination_register = cpu_read_program(cpu);
    uint32_t source_address = cpu_read_program_int32(cpu);

    assert_register_exists(destination_register);
    assert_address_writable(source_address);

    cpu_set_register_value(cpu, destination_register, *cpu_mem_get(cpu, source_address));
}

MAKE_OP_HANDLER(OP_SET_REG_REG) {
    uint8_t destination_register = cpu_read_program(cpu);
    uint8_t source_register = cpu_read_program(cpu);

    assert_register_exists(destination_register);
    assert_register_exists(source_register);

    cpu_set_register_value(cpu, destination_register, cpu_get_register_value(cpu, source_register));
}

MAKE_OP_HANDLER(OP_SET_ADDR_IMMEDIATE8) {
    uint32_t destination_address = cpu_read_program_int32(cpu);
    assert_address_writable(destination_address);
    cpu_mem_set(cpu, destination_address, cpu_read_program(cpu));
}

MAKE_OP_HANDLER(OP_SET_ADDR_IMMEDIATE16) {
    uint32_t destination_address = cpu_read_program_int32(cpu);
    assert_address_writable(destination_address);
    cpu_mem_set(cpu, destination_address, cpu_read_program(cpu));
    cpu_mem_set(cpu, destination_address + 1, cpu_read_program(cpu));
}

MAKE_OP_HANDLER(OP_SET_ADDR_IMMEDIATE32) {
    uint32_t destination_address = cpu_read_program_int32(cpu);
    assert_address_writable(destination_address);
    cpu_mem_set(cpu, destination_address, cpu_read_program(cpu));
    cpu_mem_set(cpu, destination_address + 1, cpu_read_program(cpu));
    cpu_mem_set(cpu, destination_address + 2, cpu_read_program(cpu));
    cpu_mem_set(cpu, destination_address + 3, cpu_read_program(cpu));
}

MAKE_OP_HANDLER(OP_SET_ADDR_ADDR) {
    uint32_t destination_address = cpu_read_program_int32(cpu);
    uint32_t source_address = cpu_read_program_int32(cpu);

    assert_address_writable(destination_address);
    assert_address_readable(source_address);

    cpu_mem_set(cpu, destination_address, *cpu_mem_get(cpu, source_address));
}

MAKE_OP_HANDLER(OP_SET_ADDR_REG) {
    uint32_t destination_address = cpu_read_program_int32(cpu);
    uint8_t source_register = cpu_read_program(cpu);

    assert_address_writable(destination_address);
    assert_register_exists(source_register);

    int32_t value = cpu_get_register_value(cpu, source_register);

    cpu_mem_set(cpu, destination_address, value);
    cpu_mem_set(cpu, destination_address + 1, value >> 8);
    cpu_mem_set(cpu, destination_address + 2, value >> 16);
    cpu_mem_set(cpu, destination_address + 3, value >> 24);
}

MAKE_OP_HANDLER(OP_SET_RADDR_RADDR) {
    uint8_t destination_register = cpu_read_program(cpu);
    uint8_t source_register = cpu_read_program(cpu);

    assert_register_exists(destination_register);
    assert_register_exists(source_register);

    uint32_t destination_address = cpu_get_register_value(cpu, destination_register);
    uint32_t source_address = cpu_get_register_value(cpu, source_register);

    assert_address_writable(destination_address);
    assert_address_readable(source_address);

    cpu_mem_set(cpu, destination_address, *cpu_mem_get(cpu, source_address));
}

MAKE_OP_HANDLER(OP_SET_RADDR_IMMEDIATE8) {
    uint8_t destination_register = cpu_read_program(cpu);
    assert_register_exists(destination_register);

    uint32_t destination_address = cpu_get_register_value(cpu, destination_register);
    assert_address_writable(destination_address);

    uint8_t value = cpu_read_program(cpu);

    cpu_mem_set(cpu, destination_address, value);
}

MAKE_OP_HANDLER(OP_SET_RADDR_IMMEDIATE16) {
    uint8_t destination_register = cpu_read_program(cpu);
    assert_register_exists(destination_register);

    uint32_t destination_address = cpu_get_register_value(cpu, destination_register);
    assert_address_writable(destination_address);

    uint16_t value = cpu_read_program_int16(cpu);

    cpu_mem_set(cpu, destination_address, value);
    cpu_mem_set(cpu, destination_address + 1, value >> 8);
}

MAKE_OP_HANDLER(OP_SET_RADDR_IMMEDIATE32) {
    uint8_t destination_register = cpu_read_program(cpu);
    assert_register_exists(destination_register);

    uint32_t destination_address = cpu_get_register_value(cpu, destination_register);
    assert_address_writable(destination_address);

    uint32_t value = cpu_read_program_int32(cpu);

    cpu_mem_set(cpu, destination_address, value);
    cpu_mem_set(cpu, destination_address + 1, value >> 8);
    cpu_mem_set(cpu, destination_address + 1, value >> 16);
    cpu_mem_set(cpu, destination_address + 1, value >> 24);
}

MAKE_OP_HANDLER(OP_JMP_RELATIVE) {
    uint32_t offset = cpu_read_program_int32(cpu);
    uint32_t address = *cpu->ip + offset;
    assert_address_jmpable(address);
    cpu_jmp(cpu, address);
}

MAKE_OP_HANDLER(OP_JMP_ABSOLUTE) {
    uint32_t address = cpu_read_program_int32(cpu);
    assert_address_jmpable(address);
    cpu_jmp(cpu, address);
}

MAKE_OP_HANDLER(OP_JMP_REG) {
    uint8_t register_index = cpu_read_program(cpu);
    assert_register_exists(register_index);
    uint32_t address = cpu_get_register_value(cpu, register_index);
    assert_address_jmpable(address);
    cpu_jmp(cpu, address);
}

MAKE_OP_HANDLER(OP_JMP_IF_NOT_EQUAL) {
    uint32_t address = cpu_read_program_int32(cpu);
    assert_address_jmpable(address);
    if (*cpu->flag_equal == 0) {
        cpu_jmp(cpu, address);
    }
}

MAKE_OP_HANDLER(OP_CMP_REG_IMMEDIATE) {
    uint8_t register_index = cpu_read_program(cpu);
    assert_register_exists(register_index);
    uint32_t value = cpu_read_program_int32(cpu);

    if (cpu_get_register_value(cpu, register_index) == value) {
        *cpu->flag_equal = 1;
    } else {
        *cpu->flag_equal = 0;
    }
}

MAKE_OP_HANDLER(OP_INCREMENT) {
    uint8_t register_index = cpu_read_program(cpu);
    assert_register_exists(register_index);
    cpu_set_register_value(cpu, register_index, cpu_get_register_value(cpu, register_index) + 1);
}

MAKE_OP_HANDLER(OP_DECREMENT) {
    uint8_t register_index = cpu_read_program(cpu);
    assert_register_exists(register_index);
    cpu_set_register_value(cpu, register_index, cpu_get_register_value(cpu, register_index) - 1);
}

MAKE_OP_HANDLER(OP_HALT) {
    cpu->running = false;
}

MAKE_OP_HANDLER(OP_ADD_REG_REG) {
    uint8_t register_a = cpu_read_program(cpu);
    uint8_t register_b = cpu_read_program(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(register_b);
    assert_register_exists(destination_register);

    cpu_set_register_value(cpu, destination_register, cpu_get_register_value(cpu, register_a) + cpu_get_register_value(cpu, register_b));
}

MAKE_OP_HANDLER(OP_ADD_REG_IMM32) {
    uint8_t register_a = cpu_read_program(cpu);
    uint32_t value = cpu_read_program_int32(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(destination_register);

    cpu_set_register_value(cpu, destination_register, cpu_get_register_value(cpu, register_a) + value);
}

MAKE_OP_HANDLER(OP_SUB_REG_REG) {
    uint8_t register_a = cpu_read_program(cpu);
    uint8_t register_b = cpu_read_program(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(register_b);
    assert_register_exists(destination_register);

    cpu_set_register_value(cpu, destination_register, cpu_get_register_value(cpu, register_a) - cpu_get_register_value(cpu, register_b));
}

MAKE_OP_HANDLER(OP_SUB_REG_IMM32) {
    uint8_t register_a = cpu_read_program(cpu);
    uint32_t value = cpu_read_program_int32(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(destination_register);

    cpu_set_register_value(cpu, destination_register, cpu_get_register_value(cpu, register_a) - value);
}

MAKE_OP_HANDLER(OP_SUB_IMM32_REG) {
    uint8_t register_a = cpu_read_program(cpu);
    uint32_t value = cpu_read_program_int32(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(destination_register);

    cpu_set_register_value(cpu, destination_register, value - cpu_get_register_value(cpu, register_a));
}

MAKE_OP_HANDLER(OP_MUL_REG_REG) {
    uint8_t register_a = cpu_read_program(cpu);
    uint8_t register_b = cpu_read_program(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(register_b);
    assert_register_exists(destination_register);

    cpu_set_register_value(cpu, destination_register, cpu_get_register_value(cpu, register_a) * cpu_get_register_value(cpu, register_b));
}

MAKE_OP_HANDLER(OP_MUL_REG_IMM32) {
    uint8_t register_a = cpu_read_program(cpu);
    uint32_t value = cpu_read_program_int32(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(destination_register);

    cpu_set_register_value(cpu, destination_register, cpu_get_register_value(cpu, register_a) * value);
}

MAKE_OP_HANDLER(OP_DIV_REG_REG) {
    uint8_t register_a = cpu_read_program(cpu);
    uint8_t register_b = cpu_read_program(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(register_b);
    assert_register_exists(destination_register);

    uint32_t divisor = cpu_get_register_value(cpu, register_a);
    uint32_t denominator = cpu_get_register_value(cpu, register_b);

    if (denominator == 0) {
        cpu_panic(cpu, "division by zero");
    }

    cpu_set_register_value(cpu, destination_register, divisor / denominator);
}

MAKE_OP_HANDLER(OP_DIV_REG_IMM32) {
    uint8_t register_a = cpu_read_program(cpu);
    uint32_t denominator = cpu_read_program_int32(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(destination_register);

    uint32_t divisor = cpu_get_register_value(cpu, register_a);

    if (denominator == 0) {
        cpu_panic(cpu, "division by zero");
    }

    cpu_set_register_value(cpu, destination_register, divisor / denominator);
}

MAKE_OP_HANDLER(OP_DIV_IMM32_REG) {
    uint8_t register_a = cpu_read_program(cpu);
    uint32_t divisor = cpu_read_program_int32(cpu);
    uint8_t destination_register = cpu_read_program(cpu);

    assert_register_exists(register_a);
    assert_register_exists(destination_register);

    uint32_t denominator = cpu_get_register_value(cpu, register_a);
    if (denominator == 0) {
        cpu_panic(cpu, "division by zero");
    }

    cpu_set_register_value(cpu, destination_register, divisor / denominator);
}

cpu_executor_t *cpu_executor_create(starkcpu_t *cpu) {
    cpu_executor_t *executor = malloc(sizeof(cpu_executor_t));
    executor->cpu = cpu;
    executor->handlers_map = opcode_handlers_map_create();
    opcode_handlers_map_reserve(executor->handlers_map, 255);

    DEFINE_OP(OP_NOP);
    DEFINE_OP(OP_SET_REG_IMMEDIATE8);
    DEFINE_OP(OP_SET_REG_IMMEDIATE16);
    DEFINE_OP(OP_SET_REG_IMMEDIATE32);
    DEFINE_OP(OP_SET_REG_ADDR);
    DEFINE_OP(OP_SET_REG_REG);
    DEFINE_OP(OP_SET_ADDR_IMMEDIATE8);
    DEFINE_OP(OP_SET_ADDR_IMMEDIATE16);
    DEFINE_OP(OP_SET_ADDR_IMMEDIATE32);
    DEFINE_OP(OP_SET_ADDR_ADDR);
    DEFINE_OP(OP_SET_ADDR_REG);
    DEFINE_OP(OP_SET_RADDR_RADDR);
    DEFINE_OP(OP_SET_RADDR_IMMEDIATE8);
    DEFINE_OP(OP_SET_RADDR_IMMEDIATE16);
    DEFINE_OP(OP_SET_RADDR_IMMEDIATE32);
    DEFINE_OP(OP_JMP_RELATIVE);
    DEFINE_OP(OP_JMP_ABSOLUTE);
    DEFINE_OP(OP_JMP_REG);
    DEFINE_OP(OP_JMP_IF_NOT_EQUAL);
    DEFINE_OP(OP_INCREMENT);
    DEFINE_OP(OP_DECREMENT);
    DEFINE_OP(OP_CMP_REG_IMMEDIATE);
    DEFINE_OP(OP_HALT);
    DEFINE_OP(OP_ADD_REG_REG);
    DEFINE_OP(OP_ADD_REG_IMM32);
    DEFINE_OP(OP_SUB_REG_REG);
    DEFINE_OP(OP_SUB_REG_IMM32);
    DEFINE_OP(OP_SUB_IMM32_REG);
    DEFINE_OP(OP_MUL_REG_REG);
    DEFINE_OP(OP_MUL_REG_IMM32);
    DEFINE_OP(OP_DIV_REG_REG);
    DEFINE_OP(OP_DIV_REG_IMM32);
    DEFINE_OP(OP_DIV_IMM32_REG);

    return executor;
}

void cpu_execute_instruction(cpu_executor_t *executor, uint8_t code) {
    opcode_exec_func handler = (opcode_exec_func) *opcode_handlers_map_get(executor->handlers_map, code);
    if (handler) {
        handler(executor->cpu);
    } else {
        cpu_panic(executor->cpu, "unknown opcode encountered: 0x%02X", code);
    }
}