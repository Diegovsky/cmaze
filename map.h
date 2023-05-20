#pragma once

#include "stdbool.h"
#include "vector.h"
#include <stddef.h>
typedef long block_t;

typedef struct {
    block_t *blocks;
    size_t width;
    size_t height;
} map_t ;


map_t *map_create(size_t width, size_t height);
bool map_get(map_t *map, size_t x, size_t y);
bool map_get_v2(map_t *map, v2 pos);
void map_set(map_t *map, size_t x, size_t y, block_t value);
void map_set_v2(map_t *map, v2 pos, block_t value);
