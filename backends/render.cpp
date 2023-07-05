#include "backends/render.hpp"
#include <cstdio>
#include <stdint.h>


extern "C" void renderer_render_blocks(renderer_t* render, block_t* blocks, v2 res) {
    render->render_blocks(blocks, res);
}

extern "C" v2   renderer_get_resolution(renderer_t* render) {
    return render->get_resolution();
}

extern "C" bool renderer_should_close(renderer_t* render) {
    return render->should_close();
}
