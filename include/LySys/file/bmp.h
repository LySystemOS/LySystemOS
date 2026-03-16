#ifndef LYSYS_BMP_H
#define LYSYS_BMP_H

#include <LySys/types.h>

#pragma pack(push, 1)

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} bmp_file_header_t;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount; 
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} bmp_info_header_t;

#pragma pack(pop)

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t *pixels;
} bmp_image_t;

bmp_image_t* bmp_parse(uint8_t* data, uint64_t size);
void bmp_draw(bmp_image_t* img, uint32_t x, uint32_t y);
void bmp_free(bmp_image_t* img);

#endif