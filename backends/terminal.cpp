extern "C" {
#include "backends/render.h"
#include "backends/terminal.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
}

#include <new>
#include "backends/render.hpp"

struct TermBackend: renderer {
public:
    TermBackend() {
    }
    void render_blocks(map_t* map) override {
        block_t* blocks = map->blocks;
        int width = map->width;
        int height = map->height;
        printf("\33[0;0H");
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%c", !blocks[y * width + x] ? '#' : ' ');
            }
            printf("\n");
        }
    }
    v2   get_resolution() override {
        v2 res;
        struct winsize w;

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        res.x = w.ws_col;
        res.y = w.ws_row-1;
        return res;
    }
};



extern "C" renderer_t* renderer_new_terminal() {
    TermBackend* term = new(malloc(sizeof(TermBackend))) TermBackend();
    return term;
}
