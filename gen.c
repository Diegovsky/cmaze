#include <string.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

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
    v2 rnextpos = vadd(nextpos, dir_to_vector(dir_add(dir, 1)));
    return map_get_v2(map, nextpos) && map_get_v2(map, lnextpos) && map_get_v2(map, rnextpos);
}

static void resize_stack(random_map_data_t* data, size_t new_cap) {
    data->stack = realloc(data->stack, sizeof(map_gen_frame_t) * new_cap);
    data->stack_cap = new_cap;
}

static void create_random_walls(random_map_data_t* data) {
    map_t* map = data->map;
    resize_stack(data, 64);
    data->stack_len = 0;
    while(true) {
        start:;
        map_gen_frame_t* current_frame = &data->stack[data->stack_len];
        v2 pos = current_frame->pos;
        data->on_update(data);
        enum Direction dir = dir_random_turn(current_frame->dir);
        int newdirsign = randint(0, 1) ? 1 : -1;
        int id = data->id;
        // try random directions until one of them is valid
        for (int i = 0; i < 5; i++) {
            if (wall_can_die(map, pos, dir)) {
                break;
            }
            dir = dir_add(dir, newdirsign);
            if (i == 4) {
                data->stack_len--;
                goto start;
            }
        }
        v2 vdir = dir_to_vector(dir);
        int i = 0;
        int maxlen = randint(1, 4)*2;
        while(wall_can_die(map, pos, dir)) {
            map_set_v2(map, pos, id);
            v2 newpos = vadd(pos, vdir);

            // fork
            if(i & 2) {
                data->id ++;
                create_random_walls(data, pos, dir);
                create_random_walls(data, pos, dir);
                data->id = id;
            }
            pos = newpos;
            if(i++ == maxlen) break;
        }
    }
    end:
    free(data->stack);
    data->stack_len = 0;
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
        .id = 0,
    };


    // map_set_walls(map);

    create_random_walls(&data, (v2){1, 1}, UP);
    create_random_walls(&data, (v2){1, map->height/2}, LEFT);
    create_random_walls(&data, (v2){1, map->height-2}, LEFT);


    map_set_v2(map, data.exit_down, false);
    map_set_v2(map, data.exit_up, false);

}

