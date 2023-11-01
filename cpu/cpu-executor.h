#pragma once

#include "cpu.h"
#include "opcode-handlers-map.h"

typedef struct {
    opcode_handlers_map_t *handlers_map;
    starkcpu_t *cpu;
} cpu_executor_t;

cpu_executor_t *cpu_executor_create(starkcpu_t *cpu);
void cpu_execute_instruction(cpu_executor_t *executor, uint8_t code);