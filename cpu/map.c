#include "map.h"
#include <stdlib.h>
#include <string.h>

struct map_t *find_last_entry(struct map_t *map) {
    struct map_t *node = map;
    while (node) {
        if (!node->next) {
            return node;
        }

        node = node->next;
    }

    return 0;
}

struct map_t* map_create() {
    struct map_t *map = malloc(sizeof(struct map_t));
    map->key = "\0";
    map->next = 0;
    return map;
}

void map_set(struct map_t *map, uint32_t key, void* value) {
    struct map_t *last = find_last_entry(map);
    if (last) {
        last->next = malloc(sizeof(struct map_t));
        last->next->key = key;
        last->next->value = value;
        last->next->next = 0;
    }
}

void* map_get(struct map_t *map, uint32_t key) {
    struct map_t *node = map;
    while (node) {
        if (node->key == key) {
            return node->value;
        }

        node = node->next;
    }

    return 0;
}

bool map_has(struct map_t *map, uint32_t key) {
    struct map_t *node = map;
    while (node) {
        if (node->key == key) {
            return true;
        }

        node = node->next;
    }

    return false;
}