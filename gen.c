#include <string.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "gen.h"
#include "map.h"

#define printint(X) ({int Y = X; printf(#X": %d\n", Y); Y;})

bool randbool() {
    return rand() % 2;
}

int randint(int min, int max) {
    if (min >= max) {
        exit(-1);
    }
    return rand() % (max - min+1) + min;
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

static inline enum Direction dir_add(enum Direction dir, int i) {
    return ((dir) + (i)) % DIRECTION_COUNT;
}

inline static bool dir_is_opposite(enum Direction dir1, enum Direction dir2) {
    return dir_add(dir1, 2) == dir2;
}

enum Direction dir_random_turn(enum Direction dir) {
    return randint(0, 1) ? dir_add(dir, 1) : dir_add(dir, -1);
}

static bool wall_can_die(map_t* map, v2 pos, enum Direction dir) {
    v2 nextpos = vadd(pos, dir_to_vector(dir));
    v2 lnextpos = vadd(nextpos, dir_to_vector(dir_add(dir, 1)));
    v2 rnextpos = vadd(nextpos, dir_to_vector(dir_add(dir, -1)));
    return map_get_v2(map, nextpos) && map_get_v2(map, lnextpos) && map_get_v2(map, rnextpos);
}

static void resize_stack(random_map_data_t* data, size_t new_cap) {
    if(new_cap == 0 && data->stack_cap == 0) {
        new_cap = 128;
    }
    assert(new_cap > data->stack_cap);
    /* FILE* fp = fopen("log.txt", "a");
    fprintf(fp, "%lu\n", new_cap);
    fclose(fp); */
    data->stack = realloc(data->stack, sizeof(map_gen_frame_t) * new_cap);
    data->stack_cap = new_cap;
}

static map_gen_frame_t* current_frame(random_map_data_t* data) {
    if(data->stack_len >= 1) {
        return &data->stack[data->stack_len-1];
    }
    return NULL;
}

typedef enum {
    CLOSE,
    CONTINUE,
} ClosureStatus;

static void push_frame(random_map_data_t* data, enum Direction dir, v2 start, block_t id) {
    if (data->stack_len >= data->stack_cap) {
        resize_stack(data, data->stack_cap*8);
    }
    int maxlen = randint(3, 8);
    map_set_v2(data->map, vadd(start, dir_to_vector(dir)), -2);
    map_gen_frame_t new_frame = {.dir=dir, .id=id, .pos=start, .i=0, .maxlen=maxlen};
    data->stack[data->stack_len++] = new_frame;
}

static void fork_path(random_map_data_t* data) {
    map_gen_frame_t* frame = current_frame(data);
    const int id = frame->id;
    const v2 pos = frame->pos;

    enum Direction newdir = dir_random_turn(frame->dir);
    assert(newdir != frame->dir);
    push_frame(data, newdir, pos, id+1);
    frame = current_frame(data);

    newdir = dir_add(newdir, 2);
    assert(newdir != frame->dir);
    push_frame(data, newdir, pos, id+1);

}

static ClosureStatus closure(random_map_data_t* data) {
    map_gen_frame_t* frame = current_frame(data);
    v2* pos = &frame->pos;
    v2 vdir = dir_to_vector(frame->dir);
    data->on_update(data);
    int cur_index = data->stack_len-1;
    while(frame->i++ < frame->maxlen && wall_can_die(data->map, *pos, frame->dir)) {
        map_set_v2(data->map, *pos, frame->id);
        if((frame->i-1)&2) {
            fork_path(data);
            frame = current_frame(data);
            pos = &frame->pos;
            return CONTINUE;
        }
        *pos = vadd(*pos, vdir);
    }
    map_set_v2(data->map, *pos, -1);
    return CLOSE;
}

static void create_random_walls(random_map_data_t* data, v2 start) {
    data->stack_len = 0;
    push_frame(data, randint(0, DIRECTION_COUNT-1), start, 0);
    while(data->stack_len > 0) {
        if(closure(data) == CLOSE) {
            data->stack_len--;
        }
    }
    free(data->stack);
    data->stack = NULL;
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
        .stack = NULL,
        .stack_len = 0,
        .stack_cap = 0,
    };


    // map_set_walls(map);

    create_random_walls(&data, (v2){1, 1});
    create_random_walls(&data, (v2){map->width/2, map->height/2});
    create_random_walls(&data, (v2){1, map->height/2});
    create_random_walls(&data, (v2){1, map->height-2});


    map_set_v2(map, data.exit_down, false);
    map_set_v2(map, data.exit_up, false);

    printf("end\n");

}

