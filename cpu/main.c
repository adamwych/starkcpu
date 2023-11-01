#include <stdio.h>
#include "cpu.h"
#include "cpu-ui.h"

void execute_binary_file(starkcpu_t *cpu, const char* path) {
    FILE* file = fopen(path, "rb");

    if (!file) {
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void* data = cpu_mem_alloc_at(cpu, CPU_IMAGE_LOAD_ADDRESS, size);
    fread(data, 1, size, file);
    uint32_t offset = cpu_mem_get_block_offset(cpu, data);

    fclose(file);

    cpu_jmp(cpu, offset);
    cpu_start(cpu);
}

int main() {
    starkcpu_t *cpu = cpu_create(true);

    if (!cpu) {
        printf("unable to create cpu\n");
        return 1;
    }

    if (cpu->ui) {
        cpu_ui_load_disassembly_map(cpu->ui, "/Users/adam.wychowaniec/Projects/starkcpu/examples/test.sasm.map", "/Users/adam.wychowaniec/Projects/starkcpu/examples/test.sasm");
    }

    execute_binary_file(cpu, "/Users/adam.wychowaniec/Projects/starkcpu/examples/test.sasm.bin");

    return 0;
}
