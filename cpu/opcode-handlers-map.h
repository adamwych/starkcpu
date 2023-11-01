#pragma once

#include "cpu.h"
#include <stdint.h>

typedef void (*opcode_exec_func)(starkcpu_t *cpu);
typedef struct {
    opcode_exec_func* entries;
} opcode_handlers_map_t;

opcode_handlers_map_t *opcode_handlers_map_create();
void opcode_handlers_map_reserve(opcode_handlers_map_t *map, uint32_t num);
void opcode_handlers_map_set(opcode_handlers_map_t *map, uint32_t op, opcode_exec_func func);
opcode_exec_func *opcode_handlers_map_get(opcode_handlers_map_t *map, uint32_t key);