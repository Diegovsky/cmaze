#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "backends/bitmap.h"
#include "bitmap.h"
#include "gen.h"
#include "backends/sdl.h"
#include "backends/render.h"
#include "map.h"
#include "vector.h"

renderer_t* render;

void msleep(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

#define printint(X) ({int Y = X; printf(#X": %d\n", Y); Y;})
#define tabs(num) for(int i = 0; i < num; i++) putchar(' ')

void map_render(map_t *map) {
    renderer_render_blocks(render, map);
}

void map_set_walls(map_t* map) {

    // Up and down
    for(int i = 0; i < map->width; i++) {
        map->blocks[i] = 0;
    }
    for(int i = 0; i < map->width; i++) {
        map->blocks[map->width*(map->height - 1) + i] = 0;
    }
    // Sides
    for(int i = 0; i < map->height; i++) {
        map->blocks[i * map->width] = 0;
    }
    for(int i = 0; i < map->height; i++) {
        map->blocks[i * map->width + map->width-1] = 0;
    }
}

void map_destroy(map_t *map) {
    free(map->blocks);
    free(map);
}


#define SKIP 1000
void render_map_step(random_map_data_t* data) {
    static int step = SKIP;
    if(renderer_should_close(render)) exit(-1);
    if(step-- <= 0) {
        map_render(data->map);
        step = SKIP;
    }
    return;
}


int main(int argc, char **argv) {

    float scale = 0.1;
    int initrand = 0;
    FILE* fp = fopen("/dev/urandom", "rb");
    if (fp) {
        fread(&initrand, sizeof(initrand), 1, fp);
        fclose(fp);
    }
    srand(time(NULL) ^ initrand);
    if (argc > 1) {
        scale = atof(argv[1]);
    }
    // render = renderer_new_sdl(0.8);
    render = renderer_new_bitmap(1920, 1080);
    v2 res = renderer_get_resolution(render);
    map_t *map = map_create(res.x, res.y);
    map_render(map);
    map_create_random(map, scale, 2000, render_map_step);
    map_render(map);
    renderer_free(render);
    map_destroy(map);
    return 0;
}
