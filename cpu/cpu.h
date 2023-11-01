#pragma once

#include <stdint.h>
#include <stdbool.h>

#define CPU_RESERVED_MEMORY_SIZE 256
#define CPU_IMAGE_LOAD_ADDRESS 0x00000100
#define CPU_TICK_PER_SECOND 2
#define CPU_UI_UPDATE_PER_SECOND 3

typedef struct {
    char* mem;
    char* nextmem;
    uint32_t memsize;
    bool running;
    void *ui;

    uint8_t* version;
    uint8_t* model;
    uint32_t* ip;
    int32_t* reg_a;
    int32_t* reg_b;
    int32_t* reg_c;
    uint8_t* flag_equal;
} starkcpu_t;

starkcpu_t* cpu_create(bool with_ui);
char* cpu_mem_alloc(starkcpu_t *cpu, uint32_t size);
char* cpu_mem_alloc_at(starkcpu_t *cpu, uint32_t start, uint32_t size);
void cpu_mem_set(starkcpu_t *cpu, uint32_t position, char value);
char* cpu_mem_get(starkcpu_t *cpu, uint32_t position);
uint32_t cpu_mem_get_block_offset(starkcpu_t *cpu, char* block);

void cpu_jmp(starkcpu_t *cpu, uint32_t position);

void cpu_start(starkcpu_t *cpu);

void cpu_dump_memory(starkcpu_t *cpu);

void cpu_set_register_value(starkcpu_t *cpu, uint8_t index, int32_t value);
int32_t cpu_get_register_value(starkcpu_t *cpu, uint8_t index);

uint8_t cpu_read_program(starkcpu_t *cpu);
uint16_t cpu_read_program_int16(starkcpu_t *cpu);
uint32_t cpu_read_program_int32(starkcpu_t *cpu);

void cpu_panic(starkcpu_t *cpu, char* message, ...);