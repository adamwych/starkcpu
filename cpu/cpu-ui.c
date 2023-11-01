#include "cpu-ui.h"
#include "cpu.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* get_formatted_register_value(starkcpu_t *cpu, int index) {
    int32_t* ptr = cpu->reg_a + index;
    char val = *ptr;
    char* out2 = malloc(sizeof(char) * 12);
    sprintf(out2, "R%d=%08X", index, val);
    return out2;
}

cpu_ui_t *cpu_ui_initialize(starkcpu_t *cpu) {
    cpu_ui_t *ui = malloc(sizeof(cpu_ui_t));
    ui->cpu = cpu;
    ui->mem_snapshot = malloc(sizeof(char) * ui->cpu->memsize);
    ui->disassembly_map = map_create();
    ui->screen = initscr();

    cbreak();
    noecho();
    clear();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    init_pair(3, 8, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);

    return ui;
}

void cpu_ui_draw_memory_panel(cpu_ui_t *ui) {
    uint32_t height = getmaxy(ui->screen);

    attron(COLOR_PAIR(2));
    mvaddstr(0, 0, "                       MEMORY                       ");

    const int cols = 16;

    for (int y = 0; y < 4; y++) {
        attron(COLOR_PAIR(3));
        char* out = malloc(sizeof(char) * 5);
        sprintf(out, "%04X", y * cols);
        mvaddstr(y + 1, 0, out);
        free(out);

        attron(COLOR_PAIR(1));
        for (int x = 0; x < cols; x++) {
            int mem_offset = (y * cols + x);
            char mem_value = *cpu_mem_get(ui->cpu, mem_offset);
            char* out2 = malloc(sizeof(char) * 3);
            sprintf(out2, "%02X", mem_value & 0xFF);

            if (mem_value != ui->mem_snapshot[mem_offset]) {
                attron(COLOR_PAIR(4));
            } else {
                attron(COLOR_PAIR(1));
            }

            mvaddstr(y + 1, (x * 3) + 5, out2);
            free(out2);
        }
    }

    attron(COLOR_PAIR(3));
    mvaddstr(5, 0, "...");

    for (int y = 0; y < height - 4; y++) {
        attron(COLOR_PAIR(3));
        char* out = malloc(sizeof(char) * 5);
        sprintf(out, "%04X", CPU_RESERVED_MEMORY_SIZE + 1024 + (y * cols));
        mvaddstr(y + 1 + 5, 0, out);
        free(out);

        for (int x = 0; x < cols; x++) {
            int mem_offset = CPU_RESERVED_MEMORY_SIZE + 1024 + (y * cols + x);
            char mem_value = *cpu_mem_get(ui->cpu, mem_offset);
            char* out2 = malloc(sizeof(char) * 3);
            sprintf(out2, "%02X", mem_value & 0xFF);

            if (mem_value != ui->mem_snapshot[mem_offset]) {
                attron(COLOR_PAIR(4));
            } else {
                attron(COLOR_PAIR(1));
            }

            mvaddstr(y + 1 + 5, (x * 3) + 5, out2);
            free(out2);
        }
    }
}

void cpu_ui_draw_state(cpu_ui_t *ui) {
    attron(COLOR_PAIR(2));
    mvaddstr(0, 53, "    STATE    ");

    attron(COLOR_PAIR(1));
    for (int i = 0; i < 3; i++) {
        char* formatted_value = get_formatted_register_value(ui->cpu, i);
        mvaddstr(1 + i, 53, formatted_value);
        free(formatted_value);
    }
}

void cpu_ui_draw_code_disassembly(cpu_ui_t *ui) {
    const int basex = 53 + 14;

    attron(COLOR_PAIR(2));
    mvaddstr(0, basex, "                DISASSEMBLY               ");

    uint32_t offset = *ui->cpu->ip - CPU_IMAGE_LOAD_ADDRESS;

    attron(COLOR_PAIR(1));

    struct string_array_t *sl = ui->source_line;

    int i = 1;
    while (sl) {
        attron(COLOR_PAIR(3));
        char* formatted = itoa2(i, 10);
        if (i < 10) {
            mvaddstr(1 + i - 1, basex + 1, formatted);
        } else {
            mvaddstr(1 + i - 1, basex, formatted);
        }

        if (map_has(ui->disassembly_map, offset) && map_get(ui->disassembly_map, offset) == i - 1) {
            attron(COLOR_PAIR(2));
        } else {
            attron(COLOR_PAIR(1));
        }

        mvaddstr(1 + i - 1, basex + 3, sl->value);
        sl = sl->next;
        i++;
        free(formatted);
    }
}

void cpu_ui_redraw(cpu_ui_t *ui) {
    cpu_ui_draw_code_disassembly(ui);
    cpu_ui_draw_memory_panel(ui);
    cpu_ui_draw_state(ui);

    refresh();
    memcpy(ui->mem_snapshot, ui->cpu->mem, ui->cpu->memsize);
}

void cpu_ui_draw_text(cpu_ui_t *ui, int x, int y, char* text) {
    mvaddstr(y, x, text);
    refresh();
}

char* read_file(const char *path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = malloc(sizeof(char) * size);
    fread(data, 1, size, file);
    fclose(file);

    return data;
}

void cpu_ui_load_disassembly_map(cpu_ui_t *ui, const char *map_path, const char *source_path) {
    if (ui->disassembly_map) {
        free(ui->disassembly_map);
    }

    ui->disassembly_map = map_create();

    char* map_contents = read_file(map_path);
    char* source_contents = read_file(source_path);

    if (!map_contents) {
        printf("error: File %s does not exist.\n", map_path);
        exit(1);
    }

    if (!source_contents) {
        printf("error: File %s does not exist.\n", source_path);
        exit(1);
    }

    ui->source_line = split_buffer_by_char(source_contents, strlen(source_contents), '\n');

    struct string_array_t *line = split_buffer_by_char(map_contents, strlen(map_contents), '\n');
    while (line) {
        struct string_array_t *val = split_buffer_by_char(line->value, line->length, '|');

        if (val && val->next) {
            int offset = atoi(val->value);
            int line_index = atoi(val->next->value);
            map_set(ui->disassembly_map, offset, (void *) line_index);
        }

        line = line->next;
    }

    free(map_contents);
    free(source_contents);
}