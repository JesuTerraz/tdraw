#ifndef DRAW_H
#define DRAW_H

#include "pose.h"

#define COLORLEN 5 // Length of escape sequence for setting colors.
#define PIXELLEN ((2 * COLORLEN) + 1) // (Num escape sequences * COLORLEN ) + c

typedef enum {
    DRAW,
    OCCUPY,
    ORDER,
} DrawOptions;

typedef enum {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
} PixelColor;

typedef struct {
    int z;                      // Z-Axis ordering.
    char buf[PIXELLEN + 1];
} Pixel;

/* Set after calling init_scr()*/
extern Pose WINSIZE;
extern char *FILL;
extern Pixel *FILL_PIXEL;

void init_scr(DrawOptions dopts);
void destory_scr();
void draw(void);

int set_pixel(const Pixel *pix, Pose pose);
int remove_pixel(const Pixel *pix, Pose pose);
Pixel *get_pixel(Pose p);
Pixel *create_pixel(char c, PixelColor p, int z);

#endif