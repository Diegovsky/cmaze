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


void  renderer_render_blocks(renderer_t* render, block_t* blocks, v2 res);
v2    renderer_get_resolution(renderer_t* render);
int   renderer_get_key(renderer_t* render);
