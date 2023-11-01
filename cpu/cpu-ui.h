#pragma once

#include "cpu.h"
#include "map.h"
#include "utils.h"

#ifdef _WIN32
#include <curses.h>
#else
#include <ncurses.h>
#endif

typedef struct {
    starkcpu_t *cpu;
    WINDOW *screen;
    char *mem_snapshot;
    struct string_array_t *source_line;
    struct map_t *disassembly_map;
} cpu_ui_t;

cpu_ui_t *cpu_ui_initialize(starkcpu_t *cpu);
void cpu_ui_redraw(cpu_ui_t *ui);
void cpu_ui_draw_text(cpu_ui_t *ui, int x, int y, char* text);
void cpu_ui_load_disassembly_map(cpu_ui_t *ui, const char *map_path, const char *source_path);