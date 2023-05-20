#pragma once

#include "map.h"
#include "vector.h"

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    DIRECTION_COUNT
};

typedef struct {
    enum Direction dir;
    block_t id; 
    v2 pos;
    int i;
    int maxlen;
} map_gen_frame_t;

struct random_map_data;
typedef void (*random_map_update)(struct random_map_data* data);

typedef struct random_map_data {
    map_t* map;
    v2 exit_up, exit_down;
    random_map_update on_update;
    int fps;
    map_gen_frame_t *stack;
    size_t stack_cap;
    size_t stack_len;
} random_map_data_t;

void map_create_random(map_t *map, float scale, int fps, random_map_update on_update);
