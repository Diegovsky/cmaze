#include <assert.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#define _POSIX_C_SOURCE 200809L

#include <cstdint>
#include <new>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <thread>

#include "render.hpp"

extern "C" {
    namespace C {
        #include <unistd.h>
        #include <fcntl.h>
        #include "bitmap.h"
    }
}

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);

map_t* dupe_map(map_t* map) {
    map_t* new_map = map_create(map->width, map->height);
    std::memcpy(new_map->blocks, map->blocks, sizeof(block_t) * map->width * map->height);
    return new_map;
}

uint8_t* pixels_from_map(map_t* map) {
    int width = map->width;
    int height = map->height;
    int img_res = width*height;
    uint8_t* pixels = new uint8_t[img_res * 3];
    for(int i = 0; i < img_res; i++) {
        int x = i % width;
        int y = i / width;
        int color = map->blocks[i];
        if(color > 0) {
            int r = color & 0xFF;
            int g = ((float)x / width) * 255;
            int b = ((float)y / height) * 255;
            pixels[i*3 + 0] = b;
            pixels[i*3 + 1] = g;
            pixels[i*3 + 2] = r;
        } else {
            pixels[i*3 + 0] = 0;
            pixels[i*3 + 1] = 0;
            pixels[i*3 + 2] = 0;
        }
    }
    return pixels;
}

struct FrameContext {
    size_t width, height;
    uint8_t* pixels;
    bool closed = false;
    int fd;

    std::string buffer;
    int idx = 0;

    FrameContext(size_t width, size_t height, uint8_t* pixels, int fd): width(width), height(height), pixels(pixels), fd(fd) {
        std::stringstream buf;
        buf << "P6\n" << width << " " << height << "\n255\n";
        buffer = buf.str();
        for(int i = 0; i < width*height; i++) {
            buffer.push_back(pixels[i]);
        }
    }

    bool write() {
        assert(!closed);
        int bytes_read = C::write(fd, this->buffer.data() + this->idx, this->buffer.length() - this->idx);
        if (bytes_read < 0 && errno != EAGAIN) {
            perror("write");
        }
        this->idx += bytes_read;
        if (this->idx == this->buffer.length()) {
            C::close(fd);
            this->closed = true;
            return false;
        }
        return true;
    }

    ~FrameContext() {
        delete[] pixels;
    }
};

struct WorkPool {
    std::mutex queue_lock;
    std::queue<std::tuple<map_t*, int>> frame_queue;
    int frame_number = 0;

    std::thread iothread;
    std::atomic_bool running = true;

    std::string filename;

    WorkPool(const char *filename) : filename(filename) {
        iothread = std::thread(&WorkPool::iothread_main, this);
    }

    std::atomic_bool& get_running() {
        return running;
    }

    void push_frame(map_t* map) {
        std::string fname = filename+std::to_string(frame_number)+".ppm";
        int fd = C::open(fname.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_NONBLOCK, 0644);
        if(fd < 0) {
            perror(fname.c_str());
            exit(1);
        }
        {
            std::lock_guard<std::mutex> lock(queue_lock);
            frame_queue.emplace(dupe_map(map), fd);
        }
        frame_number++;
    }

    void iothread_main() {
        std::queue<FrameContext> frames;
        std::cout << "Threadpoll running" << std::endl;
        while(running) {
            if(!frames.empty()) {
                if(!frames.front().write()) {
                    frames.pop();
                }
            }
            std::lock_guard<std::mutex> lock(queue_lock);
            while(!frame_queue.empty()) {
                auto [map, fd] = frame_queue.front();
                FrameContext ctx(map->width, map->height, pixels_from_map(map), fd);
                if(ctx.write()) {
                    frames.push(ctx);
                }
                frame_queue.pop();
            }
        }
    }

    ~WorkPool() {
        running = false;
        iothread.join();
    }
};

struct BitmapRenderer: renderer_t {
    uint32_t width, height;
    WorkPool pool;
    map_t* map = nullptr;
    BitmapRenderer(uint32_t width, uint32_t height, const char* fileBase) : width(width), height(height), pool(fileBase) {}


    virtual void render_blocks(map_t* map) {
        this->map = map;
        pool.push_frame(map);
    }
    virtual v2   get_resolution() {
        return (v2){.x=int(width), .y=int(height)};
    };
    virtual bool should_close() { return false; }
    virtual ~BitmapRenderer() {
    }
};

extern "C" renderer_t* renderer_new_bitmap(uint32_t width, uint32_t height, const char* file_name) {
    std::cout << width << " " << height << std::endl;
    BitmapRenderer* self = new(malloc(sizeof(BitmapRenderer))) BitmapRenderer(width, height, file_name);
    return self;
}
