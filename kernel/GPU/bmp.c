#include <LySys/file/bmp.h>
#include <LySys/video.h>
#include <LySys/pmm.h>
#include <LySys/string.h>

extern void put_pixel(uint32_t x, uint32_t y, uint32_t color);
extern void* kmalloc(size_t size);
extern void kfree(void* ptr);

bmp_image_t* bmp_parse(uint8_t* data, uint64_t size) {
    if (!data) return NULL;

    bmp_file_header_t* fileHeader = (bmp_file_header_t*)data;
    
    if (fileHeader->bfType != 0x4D42) return NULL;

    bmp_info_header_t* infoHeader = (bmp_info_header_t*)(data + 14);

    if (infoHeader->biBitCount != 24 && infoHeader->biBitCount != 32) return NULL;
    if (infoHeader->biCompression != 0) return NULL;

    uint32_t width = infoHeader->biWidth;
    int32_t height_raw = infoHeader->biHeight;
    uint32_t height = (height_raw > 0) ? height_raw : -height_raw;
    int is_bottom_up = (height_raw > 0);

    uint8_t* pixel_data = data + fileHeader->bfOffBits;

    bmp_image_t* img = (bmp_image_t*)kmalloc(sizeof(bmp_image_t));
    img->width = width;
    img->height = height;
    
    img->pixels = (uint32_t*)kmalloc(width * height * 4);
    if (!img->pixels) {
        kfree(img);
        return NULL;
    }

    int bpp = infoHeader->biBitCount / 8; 
    int padding = (4 - (width * bpp) % 4) % 4;

    for (uint32_t y = 0; y < height; y++) {
        uint32_t target_y = is_bottom_up ? (height - 1 - y) : y;
        
        for (uint32_t x = 0; x < width; x++) {
            uint8_t b = *pixel_data++;
            uint8_t g = *pixel_data++;
            uint8_t r = *pixel_data++;
            uint8_t a = 255;

            if (bpp == 4) {
                uint8_t alpha_from_file = *pixel_data++; 
                a = 255; 
            }

            img->pixels[target_y * width + x] = (a << 24) | (r << 16) | (g << 8) | b;
        }
        pixel_data += padding;
    }

    return img;
}

void bmp_draw(bmp_image_t* img, uint32_t start_x, uint32_t start_y) {
    if (!img || !img->pixels) return;

    for (uint32_t y = 0; y < img->height; y++) {
        for (uint32_t x = 0; x < img->width; x++) {
            uint32_t color = img->pixels[y * img->width + x];
            
            if ((color >> 24) == 0) continue;

            put_pixel(start_x + x, start_y + y, color & 0xFFFFFF);
        }
    }
}

void bmp_free(bmp_image_t* img) {
    if (img) {
        if (img->pixels) kfree(img->pixels);
        kfree(img);
    }
}