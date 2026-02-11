#pragma once

#include <LySys/types.h>

#define WIDTH_SCREEN    1024
#define HEIGHT_SCREEN   768

void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void clear_screen(uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void draw_char(char c, uint32_t x, uint32_t y, uint32_t color);