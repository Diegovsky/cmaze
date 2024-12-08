#pragma once

#include "backends/render.h"

renderer_t* renderer_new_bitmap(uint32_t width, uint32_t height, const char* file_name);
