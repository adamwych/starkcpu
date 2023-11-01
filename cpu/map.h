#pragma once

#include <stdint.h>
#include <stdbool.h>

struct map_t {
    uint32_t key;
    void* value;
    struct map_t *next;
};

struct map_t* map_create();
void map_set(struct map_t *map, uint32_t key, void* value);
void* map_get(struct map_t *map, uint32_t key);
bool map_has(struct map_t *map, uint32_t key);