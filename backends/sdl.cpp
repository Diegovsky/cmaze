extern "C" {
#include <SDL2/SDL_rect.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "SDL_render.h"
#include "backends/render.h"
#include "backends/sdl.h"

#include "vector.h"
}

#include <new>
#include <deque>
#include "backends/render.hpp"

#define CENTER_OFFSET 10

#define COLOR_WALL 0, 0, 0, 255
#define COLOR_AIR  0, 0, 0, 255

struct SDLBackend: renderer {
    SDL_Window* window;
    SDL_Renderer* renderer;
    double pixel_size;
    std::deque<int> keys;
    

    SDLBackend(double pixel_size): pixel_size(pixel_size) {

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            printf("error initializing SDL: %s\n", SDL_GetError());
            exit(-1);
        }
        SDL_Window* win = SDL_CreateWindow("GAME",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           1000, 1000, SDL_WINDOW_RESIZABLE);
        this->window = win;
        this->renderer = SDL_CreateRenderer(win, -1, 0);
    }
    void render_blocks(block_t* blocks, v2 res) override {
        // Fill screen
        SDL_RenderFillRect(this->renderer, NULL);
        SDL_SetRenderDrawColor(this->renderer, COLOR_AIR);
        size_t offset = CENTER_OFFSET/2;
        for (size_t y = 0; y < res.y; y++) {
            for (size_t x = 0; x < res.x; x++) {
                block_t value = blocks[y * res.x + x];
                if (value) {
                    if(value < 0) {
                        if(value == -1)
                            SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
                        else if(value == -2)
                            SDL_SetRenderDrawColor(this->renderer, 127, 127, 255, 255);
                        else
                            SDL_SetRenderDrawColor(this->renderer, COLOR_WALL);
                    } else {
                        int color = blocks[y * res.x + x];
                        int r = color & 0xFF;
                        int g = ((float)x / res.x) * 255;
                        int b = ((float)y / res.y) * 255;
                        SDL_SetRenderDrawColor(this->renderer, r, g, b, 255);
                    }
                } else {
                    SDL_SetRenderDrawColor(this->renderer, COLOR_WALL);
                }
                SDL_Rect rect = {
                    .x = int((x + offset) * (this->pixel_size)),
                    .y = int((y + offset) * (this->pixel_size)),
                    .w = int(this->pixel_size),
                    .h = int(this->pixel_size),
                };
                SDL_RenderFillRect(this->renderer, &rect);
            }
        }

        SDL_RenderPresent(this->renderer);
    }
    v2 get_resolution() override {
        v2 res;
        SDL_GetWindowSize(this->window, &res.x, &res.y);
        res.x /= this->pixel_size;
        res.y /= this->pixel_size;
        res = vsub(res, vsplat(CENTER_OFFSET));
        return printv2(res);
    }
    bool should_close() override {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    return true;
                case SDL_KEYDOWN:
                    this->keys.push_back(event.key.keysym.sym);
                    break;
            }
        }
        return false;
    };
    int get_key() override {
        if (this->keys.size()) return 0;
        int r = this->keys[0];
        this->keys.pop_front();
        return r;
    }
};




extern "C" renderer_t* renderer_new_sdl(double pixel_size) {
    SDLBackend* sdl = new(malloc(sizeof(SDLBackend))) SDLBackend(pixel_size);
    return sdl;
}
