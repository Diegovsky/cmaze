#include "map.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


map_t *map_create(size_t width, size_t height) {
    map_t *map = malloc(sizeof(map_t));
    map->width  = width;
    map->height = height;
    map->blocks = calloc(sizeof(block_t), width * height);
    return map;
}

bool map_get(map_t *map, size_t x, size_t y) {
    if (x >= map->width || y >= map->height || x < 0 || y < 0) {
        return false;
    }
    block_t val = map->blocks[y * map->width + x];
    return val <= 0;
}

bool map_get_v2(map_t *map, v2 pos) {
    return map_get(map, pos.x, pos.y);
}

void map_set(map_t *map, size_t x, size_t y, block_t value) {
    if (x >= map->width || y >= map->height || x < 0 || y < 0) {
        return;
    }
    block_t* val = &map->blocks[y * map->width + x];
    if((*val == -2 && value == -1) || (*val > 0)) return;
    *val = value;
}

void map_set_v2(map_t *map, v2 pos, block_t value) {
    map_set(map, pos.x, pos.y, value);
}
