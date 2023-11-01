#include "opcode-handlers-map.h"
#include <stdlib.h>

opcode_handlers_map_t *opcode_handlers_map_create() {
    opcode_handlers_map_t *map = malloc(sizeof(opcode_handlers_map_t));
    map->entries = 0;
    return map;
}

void opcode_handlers_map_reserve(opcode_handlers_map_t *map, uint32_t num) {
    map->entries = malloc(sizeof(opcode_exec_func) * num);

    for (int32_t i = 0; i < num; i++) {
        opcode_exec_func *ptr = map->entries + (i);
        *ptr = 0;
    }
}

void opcode_handlers_map_set(opcode_handlers_map_t *map, uint32_t key, opcode_exec_func data) {
    uint32_t index = key;
    opcode_exec_func *ptr = map->entries + (index);
    *ptr = data;
}

opcode_exec_func *opcode_handlers_map_get(opcode_handlers_map_t *map, uint32_t key) {
    uint32_t index = key;
    return map->entries + (index);
}