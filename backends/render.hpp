#pragma once

extern "C" {
#include "backends/render.h"
}

struct renderer {
public:
    virtual void render_blocks(map_t* map) = 0;
    virtual v2   get_resolution() = 0;
    virtual bool should_close() { return false; }
    virtual ~renderer() {};
};
