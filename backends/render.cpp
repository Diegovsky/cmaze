#include "backends/render.hpp"
#include <cstdio>
#include <stdint.h>


extern "C" void renderer_render_blocks(renderer_t* render, map_t* map) {
    render->render_blocks(map);
}

extern "C" v2   renderer_get_resolution(renderer_t* render) {
    return render->get_resolution();
}

extern "C" bool renderer_should_close(renderer_t* render) {
    return render->should_close();
}

extern "C" void renderer_free(renderer_t* render) {
    delete render;
}
