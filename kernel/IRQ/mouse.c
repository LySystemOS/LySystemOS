#include <LySys/LySystem.h>
#include <LySys/types.h>
#include <asm/io.h>
#include <LySys/video.h>

int mouse_old_x = WIDTH_SCREEN / 2;
int mouse_old_y = HEIGHT_SCREEN / 2;
int mouse_x = WIDTH_SCREEN / 2;
int mouse_y = HEIGHT_SCREEN / 2;