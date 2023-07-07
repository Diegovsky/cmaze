#include <cstdint>
#include <new>
#include <cstdlib>
#include <cstdio>

#include "render.hpp"

extern "C" {
    #include "bitmap.h"
}

void generateBitmapImage(unsigned char* image, int height, int width, char* imageFileName);
unsigned char* createBitmapFileHeader(int height, int stride);
unsigned char* createBitmapInfoHeader(int height, int width);


struct BitmapRenderer: renderer_t {
    uint32_t width, height;
    map_t* map = nullptr;
    BitmapRenderer(uint32_t width, uint32_t height) : width(width), height(height) {}
    virtual void render_blocks(map_t* map) {
        this->map = map;
    }
    virtual v2   get_resolution() {
        return (v2){.x=int(width), .y=int(height)};
    };
    virtual bool should_close() { return false; }
    virtual ~BitmapRenderer() {
        block_t* blocks = map->blocks;
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
    }
};

extern "C" renderer_t* renderer_new_bitmap(uint32_t width, uint32_t height) {
    BitmapRenderer* self = new(malloc(sizeof(BitmapRenderer))) BitmapRenderer(width, height);
    return self;
}

const int BYTES_PER_PIXEL = 3; /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;


void generateBitmapImage (unsigned char* image, int height, int width, char* imageFileName)
{
    int widthInBytes = width * BYTES_PER_PIXEL;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (widthInBytes) % 4) % 4;

    int stride = (widthInBytes) + paddingSize;

    FILE* imageFile = fopen(imageFileName, "wb");

    unsigned char* fileHeader = createBitmapFileHeader(height, stride);
    fwrite(fileHeader, 1, FILE_HEADER_SIZE, imageFile);

    unsigned char* infoHeader = createBitmapInfoHeader(height, width);
    fwrite(infoHeader, 1, INFO_HEADER_SIZE, imageFile);

    int i;
    for (i = 0; i < height; i++) {
        fwrite(image + (i*widthInBytes), BYTES_PER_PIXEL, width, imageFile);
        fwrite(padding, 1, paddingSize, imageFile);
    }

    fclose(imageFile);
}

unsigned char* createBitmapFileHeader (int height, int stride)
{
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

    static unsigned char fileHeader[] = {
        0,0,     /// signature
        0,0,0,0, /// image file size in bytes
        0,0,0,0, /// reserved
        0,0,0,0, /// start of pixel array
    };

    fileHeader[ 0] = (unsigned char)('B');
    fileHeader[ 1] = (unsigned char)('M');
    fileHeader[ 2] = (unsigned char)(fileSize      );
    fileHeader[ 3] = (unsigned char)(fileSize >>  8);
    fileHeader[ 4] = (unsigned char)(fileSize >> 16);
    fileHeader[ 5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader (int height, int width)
{
    static unsigned char infoHeader[] = {
        0,0,0,0, /// header size
        0,0,0,0, /// image width
        0,0,0,0, /// image height
        0,0,     /// number of color planes
        0,0,     /// bits per pixel
        0,0,0,0, /// compression
        0,0,0,0, /// image size
        0,0,0,0, /// horizontal resolution
        0,0,0,0, /// vertical resolution
        0,0,0,0, /// colors in color table
        0,0,0,0, /// important color count
    };

    height = -height;
    infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[ 4] = (unsigned char)(width      );
    infoHeader[ 5] = (unsigned char)(width >>  8);
    infoHeader[ 6] = (unsigned char)(width >> 16);
    infoHeader[ 7] = (unsigned char)(width >> 24);
    infoHeader[ 8] = (unsigned char)(height      );
    infoHeader[ 9] = (unsigned char)(height >>  8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    return infoHeader;
}
