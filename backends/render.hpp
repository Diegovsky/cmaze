#pragma once

extern "C" {
#include "backends/render.h"
}

struct renderer {
public:
    virtual void render_blocks(block_t* blocks, v2 res) = 0;
    virtual v2   get_resolution() = 0;
    virtual bool should_close() { return false; }
    virtual int get_key() = 0;
};
