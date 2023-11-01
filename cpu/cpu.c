#include "cpu.h"
#include "cpu-ui.h"
#include "cpu-executor.h"
#include "../shared/stark1-opcodes.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

static cpu_executor_t *executor;

// ==========================================
// Memory layout:
// ---------|--------------------------------
// Offset   | Description
// ---------|--------------------------------
// 0x00     | CPU model
// 0x01     | CPU version
// 0x02     | Instruction Pointer
// 0x06     | Register A
// 0x10     | Register B
// 0x14     | Register C
// ==========================================
void cpu_allocate_internal_memory(starkcpu_t *cpu) {
    // clear entire memory so we are in a known state
    for (int32_t position = 0; position < cpu->memsize; position++) {
        cpu_mem_set(cpu, position, 0);
    }

    cpu->model = (uint8_t *) cpu_mem_alloc(cpu, 1);
    cpu->version = (uint8_t *) cpu_mem_alloc(cpu, 1);
    cpu->ip = (uint32_t *) cpu_mem_alloc(cpu, 4);
    cpu->reg_a = (int32_t *) cpu_mem_alloc(cpu, 4);
    cpu->reg_b = (int32_t *) cpu_mem_alloc(cpu, 4);
    cpu->reg_c = (int32_t *) cpu_mem_alloc(cpu, 4);
    cpu->flag_equal = (uint8_t *) cpu_mem_alloc(cpu, 1);

    *cpu->model = CPU_MODEL;
    *cpu->version = 0x01;
    *cpu->ip = 0;
    *cpu->reg_a = 0;
    *cpu->reg_b = 0;
    *cpu->reg_c = 0;
    *cpu->flag_equal = 0;
}

starkcpu_t* cpu_create(bool with_ui) {
    starkcpu_t *cpu = malloc(sizeof(starkcpu_t));

    cpu->memsize = CPU_RESERVED_MEMORY_SIZE + 2048;
    cpu->mem = malloc(cpu->memsize);
    cpu->nextmem = cpu->mem;

    if (!cpu->mem) {
        return 0;
    }

    cpu_allocate_internal_memory(cpu);
    executor = cpu_executor_create(cpu);

    if (with_ui) {
        cpu->ui = cpu_ui_initialize(cpu);
    } else {
        cpu->ui = 0;
    }

    return cpu;
}

void cpu_set_register_value(starkcpu_t *cpu, uint8_t index, int32_t value) {
    if (index > 3) {
        return;
    }

    int32_t *ptr = cpu->reg_a + index;
    *ptr = value;
}

int32_t cpu_get_register_value(starkcpu_t *cpu, uint8_t index) {
    if (index > 3) {
        return 0;
    }

    int32_t *ptr = cpu->reg_a + index;
    return *ptr;
}

uint32_t get_time() {
#ifdef _WIN32
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME system_time;
    FILETIME file_time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);

    uint64_t time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    uint64_t tv_sec = (long) ((time - EPOCH) / 10000000L);
    uint64_t tv_usec = (long) (system_time.wMilliseconds * 1000);
    int32_t s1 = (int32_t)(tv_sec) * 1000;
    int32_t s2 = (tv_usec / 1000);
    return s1 + s2;
#else
    struct timeval now;
    gettimeofday(&now, NULL);
    int32_t s1 = (int32_t)(now.tv_sec) * 1000;
    int32_t s2 = (now.tv_usec / 1000);
    return s1 + s2;
#endif
}

#ifdef _WIN32
void usleep(__int64 usec) {
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#endif

void cpu_start(starkcpu_t *cpu) {
    uint32_t last_ui_update = 0;
    cpu->running = true;

    uint32_t ops = 0;
    uint32_t sleep_time = (1000 / CPU_TICK_PER_SECOND) * 1000;
    uint32_t ui_refresh_time = (1000 / CPU_UI_UPDATE_PER_SECOND);

    while (cpu->running && *cpu->ip < cpu->memsize) {
        cpu_execute_instruction(executor, cpu_read_program(cpu));

        ops++;

        uint32_t now = get_time();
        if (cpu->ui && now - last_ui_update >= ui_refresh_time) {
            cpu_ui_redraw(cpu->ui);
            cpu_ui_draw_text(cpu->ui, 0, 0, itoa2(ops, 10));
            last_ui_update = now;
            ops = 0;
        }

        if (sleep_time > 0) {
            usleep(sleep_time);
        }
    }
}

uint8_t cpu_read_program(starkcpu_t *cpu) {
    uint8_t value = *cpu_mem_get(cpu, *cpu->ip);
    *cpu->ip = *cpu->ip + 1;
    return value;
}

uint16_t cpu_read_program_int16(starkcpu_t *cpu) {
#ifdef QUICK_INT_READ
    uint16_t *value = (uint16_t*) cpu_mem_get(cpu, *cpu->ip);
    *cpu->ip = *cpu->ip + 2;
    return *value;
#else
    uint8_t value1 = cpu_read_program(cpu);
    uint8_t value2 = cpu_read_program(cpu);
    return value1 | value2 << 8;
#endif
}

uint32_t cpu_read_program_int32(starkcpu_t *cpu) {
#ifdef QUICK_INT_READ
    uint32_t *value = (uint32_t*) cpu_mem_get(cpu, *cpu->ip);
    *cpu->ip = *cpu->ip + 4;
    return *value;
#else
    uint8_t value1 = cpu_read_program(cpu);
    uint8_t value2 = cpu_read_program(cpu);
    uint8_t value3 = cpu_read_program(cpu);
    uint8_t value4 = cpu_read_program(cpu);
    return value1 | value2 << 8 | value3 << 16 | value4 << 24;
#endif
}

void cpu_jmp(starkcpu_t *cpu, uint32_t position) {
    *cpu->ip = position;
}

void cpu_dump_memory(starkcpu_t *cpu) {
    printf("------------------------------------------------------\n");
    printf("ADDR   M  V  IP          R0          R1          R2\n");
    for (int i = 0; i < 2; i++) {
        printf("%04X | ", i * 16);
        for (int j = 0; j < 16; j++) {
            char* ptr = cpu_mem_get(cpu, (i * 16 + j));
            printf("%02X ", *ptr & 0xFF);
        }

        printf("\n");
    }

    printf("....\n");
    for (int i = 0; i < 3; i++) {
        printf("%02X | ", CPU_RESERVED_MEMORY_SIZE + 1024 + i * 16);
        for (int j = 0; j < 16; j++) {
            char* ptr = cpu_mem_get(cpu, CPU_RESERVED_MEMORY_SIZE + 1024 + (i * 16 + j));
            printf("%02X ", *ptr & 0xFF);
        }

        printf("\n");
    }
    printf("------------------------------------------------------\n");
}

char* cpu_mem_alloc(starkcpu_t *cpu, uint32_t size) {
    // out of memory
    if (cpu->nextmem + size >= cpu->mem + cpu->memsize) {
        return 0;
    }

    void* ptr = cpu->nextmem;
    cpu->nextmem += size;
    return ptr;
}

char* cpu_mem_alloc_at(starkcpu_t *cpu, uint32_t start, uint32_t size) {
    // out of memory
    if (cpu->mem + start + size >= cpu->mem + cpu->memsize) {
        return 0;
    }

    void* ptr = cpu->mem + start;
    return ptr;
}

void cpu_mem_set(starkcpu_t *cpu, uint32_t position, char value) {
    char* ptr = cpu->mem + position;
    if (ptr) {
        *ptr = value;
    }
}

char* cpu_mem_get(starkcpu_t *cpu, uint32_t position) {
    if (*cpu->mem + position >= cpu->memsize) {
        cpu_panic(cpu, "attempted to read memory from 0x%02X address, which is not readable", position);
    }

    return cpu->mem + position;
}

uint32_t cpu_mem_get_block_offset(starkcpu_t *cpu, char* block) {
    return block - cpu->mem;
}

void cpu_panic(starkcpu_t *cpu, char* message, ...) {
    printf("PANIC: ");

    va_list list;
    va_start(list, message);
    vprintf(message, list);
    va_end(list);

    printf("\n");

    exit(1);
}