#ifndef DRAW_H
#define DRAW_H

#include "pose.h"

#define COLORLEN 5 // \033[3dm
#define PIXELLEN 6 // \033[3dmc

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
    char buf[PIXELLEN + 1];
} Pixel;

/* Set after calling init_scr()*/
extern Pose WINSIZE;
extern char *FILL;
extern Pixel *FILL_PIXEL;

void init_scr(void);
void draw(void);
int draw_scr(void);
int write_point(const char *buf, int buflen, Pose p);
int write_line(const char *buf, int buflen, int row);
int write_pixel(const Pixel pix, Pose pose);

Pixel *get_pixel(char c, PixelColor p);


#endif