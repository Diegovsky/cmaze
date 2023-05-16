#pragma once

#include "map.h"
#include "vector.h"

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef struct {
    enum Direction dir;
    block_t id; 
    v2 pos;
} map_gen_frame_t;

typedef struct random_map_data random_map_data_t;
typedef void (*random_map_update)(random_map_data_t* data);

struct random_map_data {
    map_t* map;
    v2 exit_up, exit_down;
    random_map_update on_update;
    int id;
    int fps;
    map_gen_frame_t *stack;
    int stack_cap;
    int stack_len;
};

void map_create_random(map_t *map, float scale, int fps, random_map_update on_update);
