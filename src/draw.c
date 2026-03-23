#include "draw.h"

#include "codes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define TO_SDX(x, y)    ((x - 1) * (rowlen)) + ((y - 1) * (PIXELLEN))
#define PIXEL_FORMAT    SET_FG_FORMAT "%c"
#define TO_FDX(x)       x * PIXELLEN

/* Source of truth for terminal winsize.*/
static struct winsize ws;
static char *scr;
static int scrlen;
static int rowlen;

char *FILL;
Pixel *FILL_PIXEL;
Pose WINSIZE;

/* 
 * Flush all printed bufs.
 * @deprecated use draw_scr instead.
 */
void draw(void) 
{
    fflush(stdout);
}

int draw_scr(void)
{
    Pose home = { 1, 1 };
    setcsr(home);
    if (write(STDOUT_FILENO, scr, scrlen) < 0) {
        return (-1);
    }

    return (0);
}

int 
write_point(const char *buf, int buflen, Pose p)
{
    if (check_bounds(p, WINSIZE)) {
        return (-1);
    }

    int sdx = TO_SDX(p.row, p.col);
    memcpy(&scr[sdx], buf, buflen);
    return (0);
}

/* Write buf to a row on the terminal.*/
int 
write_line(const char *buf, int buflen, int row)
{
    Pose p = {row, 1};
    return write_point(buf, buflen, p);
}

int
write_pixel(const Pixel pix, Pose pose)
{
    return (write_point(pix.buf, PIXELLEN, pose));
}

char *
get_fill(int idx, int len)
{
    int fdx = TO_FDX(idx);
    if (fdx < 0 || fdx + len > rowlen)
        return NULL;

    return &FILL[fdx];
}

Pixel *
get_pixel(char c, PixelColor p)
{
    Pixel *pixel = malloc(sizeof(Pixel));
    if (!pixel)
        return NULL;

    char tmp[PIXELLEN + 1]; // snprintf puts a \0 a huevo.
    snprintf(tmp, PIXELLEN + 1, PIXEL_FORMAT, p, c);

    memcpy(pixel->buf, tmp, PIXELLEN); // Leave \0 out.

    return pixel;
}

void
init_scr(void)
{
    int i;
    scrsize(&ws);

    WINSIZE.row = ws.ws_row;
    WINSIZE.col = ws.ws_col;

    raw_mode(1);
    clrscr();
    setcsr_vis(0);
    
    // Turn on full buffering: flush only when draw() or buf full.
    // We use our own buffer and write it out anyway.
    // setvbuf(stdout, NULL, _IOFBF, 0);

    // Each row should have SET_FG_FORMAT + displayed char
    rowlen = (PIXELLEN * ws.ws_col) + 1;
    FILL = malloc(rowlen);

    FILL_PIXEL = get_pixel('.', WHITE);
    for (i = 0; i < WINSIZE.col; i++) {
        int fdx = TO_FDX(i);
        memcpy(&FILL[fdx], FILL_PIXEL->buf, PIXELLEN);
    }

    FILL[rowlen - 1] = '\n';

    scrlen = WINSIZE.row * (rowlen) - 1;
    scr = malloc(scrlen + 1);
    for (i = 0; i < WINSIZE.row; i++) {
        int rdx = i * rowlen;

        memcpy(&scr[rdx], FILL, rowlen);
        if (i == WINSIZE.row - 1)
            scr[rdx + rowlen - 1] = '\0'; // last row replace \n with \0
    }

    draw_scr();

    return;
}