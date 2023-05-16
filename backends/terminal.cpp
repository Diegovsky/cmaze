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
        v2 res = get_resolution();
        // for(int i = 0; i < );
    }
    void render_blocks(block_t* blocks, v2 res) override {
        printf("\33[0;0H");
        for (int y = 0; y < res.y; y++) {
            for (int x = 0; x < res.x; x++) {
                printf("%c", !blocks[y * res.x + x] ? '#' : ' ');
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
    int get_key() override {
        return getchar();
    };
};



extern "C" renderer_t* renderer_new_terminal() {
    TermBackend* term = new(malloc(sizeof(TermBackend))) TermBackend();
    return term;
}
