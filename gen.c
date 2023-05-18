#include <string.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "gen.h"

#define dir_add(dir, i) (((dir) + (i)) % 4)

bool randbool() {
    return rand() % 2;
}

int randint(int min, int max) {
    if (min >= max) {
        exit(-1);
    }
    return rand() % (max - min) + min;
}


v2 dir_to_vector(enum Direction dir) {
    switch (dir) {
        case UP:
            return (v2){0, -1};
        case DOWN:
            return (v2){0, 1};
        case LEFT:
            return (v2){-1, 0};
        case RIGHT:
            return (v2){1, 0};
        default:
            puts("Invalid direction");
            exit(-1);
    }
}

inline static bool dir_is_opposite(enum Direction dir1, enum Direction dir2) {
    return dir_add(dir1, 2) == dir2;
}

enum Direction dir_random_turn(enum Direction dir) {
    return dir_add(dir, randbool() ? -1 : 1);
}


static bool wall_can_die(map_t* map, v2 pos, enum Direction dir) {
    v2 nextpos = vadd(dir_to_vector(dir), pos);
    v2 lnextpos = vadd(nextpos, dir_to_vector(dir_add(dir, 1)));
    v2 rnextpos = vadd(nextpos, dir_to_vector(dir_add(dir, -1)));
    return map_get_v2(map, nextpos) && map_get_v2(map, lnextpos) && map_get_v2(map, rnextpos);
}

static void resize_stack(random_map_data_t* data, size_t new_cap) {
    assert(new_cap > data->stack_cap);
    data->stack = realloc(data->stack, sizeof(map_gen_frame_t) * new_cap);
    data->stack_cap = new_cap;
}

static void push_frame(random_map_data_t* data, enum Direction dir, v2 start, block_t id) {
    if (data->stack_len+1 >= data->stack_cap) {
        resize_stack(data, data->stack_cap*2);
    }
    int maxlen = randint(1, 4)*2;
    map_gen_frame_t new_frame = {.dir=randint(0, 3), .id=id, .pos=start, .i=0, .maxlen=maxlen};
    data->stack[data->stack_len++] = new_frame;
}

static map_gen_frame_t* current_frame(random_map_data_t* data) {
    if(data->stack_len >= 1) {
        return &data->stack[data->stack_len-1];
    }
    return NULL;
}

static void create_random_walls(random_map_data_t* data, v2 start) {
    map_t* map = data->map;
    resize_stack(data, 64);
    data->stack_len = 0;
    push_frame(data, randint(0, 3), start, 0);
    while(data->stack_len > 0) {
        map_gen_frame_t* curframe = current_frame(data);
        const v2 pos = curframe->pos;
        enum Direction dir = curframe->dir;
        const int id = curframe->id;
        if (!wall_can_die(map, pos, dir)) {
            data->stack_len--;
            continue;
        }
        data->on_update(data);
        const v2 vdir = dir_to_vector(dir);
        printf("len: %d\n", data->stack_len);
        while(wall_can_die(map, pos, dir)) {
            map_set_v2(map, pos, id);

            const v2 newpos = vadd(pos, vdir);
            // fork
            if(curframe->i & 2) {
                int fork_dir = dir_random_turn(dir);
                push_frame(data, dir_add(fork_dir, -1), newpos, id+1);
                push_frame(data, dir_add(fork_dir, 1), newpos, id+1);
                goto continue_start;
            }

            curframe->pos = newpos;
            if(curframe->i++ >= curframe->maxlen){
                break;
            }
        }
        data->stack_len--;
        continue_start:;
    }
    free(data->stack);
    data->stack_len = 0;
    data->stack_cap = 0;
    return;
}

void map_create_random(map_t *map, float scale, int fps, random_map_update on_update) {
    int holeup = randint(1, map->width-1);
    int holedown = randint(1, map->width-1);

    random_map_data_t data = {
        .map = map,
        .exit_up = {.x=holeup, .y = 0},
        .exit_down = {.x=holedown, .y = map->height-1},
        .fps = fps,
        .on_update = on_update,
    };


    // map_set_walls(map);

    create_random_walls(&data, (v2){1, 1});
    create_random_walls(&data, (v2){1, map->height/2});
    create_random_walls(&data, (v2){1, map->height-2});


    map_set_v2(map, data.exit_down, false);
    map_set_v2(map, data.exit_up, false);

}

