#pragma once
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>
#include "map.h"

typedef struct renderer renderer_t;


#include "backends/terminal.h"

#ifdef HAS_SDL
#include "backends/sdl.h"
#endif


void  renderer_render_blocks(renderer_t* render, map_t* map);
v2    renderer_get_resolution(renderer_t* render);
int   renderer_get_key(renderer_t* render);
bool  renderer_should_close(renderer_t* render);
void  renderer_free(renderer_t* render);
