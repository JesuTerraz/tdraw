#include "draw.h"

#include "codes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define TO_SDX(x, y)    ((x - 1) * (rowlen)) + ((y - 1) * (PIXELLEN))
#define PIXEL_FORMAT    (SET_BFG_FORMAT SET_BG_FORMAT "%c")
#define TO_FDX(x)       (x * PIXELLEN)
#define TO_PDX(x, y)    ((x - 1) * (WINSIZE.col) + (y - 1))

/*
 * The Screen buffer to write to the terminal.
 * Use SDX to index it.
*/
static char *scr;
static int scrlen;
static int rowlen;

/*
 * Both of these are indexed by number of pixels. 
 * Use PDX to index them.
*/
/* zord: Z ordering.*/
/* oscore: Occupancy Score.*/
static int *zord, *oscore;
// static char *debugbuf;
static int debuglen;

static DrawOptions doptions;

/*
 * A Full row of the Screen Buffer.
 * Use FDX to index it.
*/
char *FILL;
Pixel *FILL_PIXEL;

/* Source of truth for terminal winsize.*/
Pose WINSIZE;

/* Private methods */

static int
draw_buf(char *buf, int buflen)
{
    Pose home = { 1, 1 };
    if (setcsr(home) < 0) 
    {
        return (-1);
    }

    int total = 0;
    while (total < buflen)
    {
        int n = write(STDOUT_FILENO, buf + total, buflen - total);

        if (n <= 0)
        {
            return (-1);
        }

        total += n;
    }

    return (0);
}

static int
draw_scr(void)
{
    return (draw_buf(scr, scrlen));
}

static int 
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
// static int 
// write_line(const char *buf, int buflen, int row)
// {
//     Pose p = {row, 1};
//     return write_point(buf, buflen, p);
// }

/* Public Methods. */

/* 
 * Flush screen buffer. This should be called after all edits have been made.
 */
void 
draw(void) 
{
    draw_scr();
}

/* Set a pixel on the buffer. */
int
set_pixel(const Pixel pix, Pose pose)
{
    int pdx;

    if (check_bounds(pose, WINSIZE))
        return (-1);

    pdx = TO_PDX(pose.row, pose.col);

    // Show interest in the spot. Z = 0 doesn't count.
    if (pix.z > 0)
        oscore[pdx] += 1;

    // Check if an existing pixel has higher priority.
    if (pix.z < zord[pdx])
        return 0;

    zord[pdx] = pix.z;

    // Default Behavior.
    if (doptions == DRAW)
        return (write_point(pix.buf, PIXELLEN, pose));

    /* Debug */
    int dchar;
    Pixel *dpix;

    switch (doptions) {
        case OCCUPY:
            dchar = oscore[pdx];
            break;
        case ORDER:
            dchar = zord[pdx];
            break;
        default:
            return (-1);
    }

    dchar = dchar > 9 ? '9' : '0' + dchar;
    dpix = create_pixel(dchar, WHITE, pix.z);
    int success = write_point(dpix->buf, PIXELLEN, pose);

    free(dpix);

    return (success);

}

/* Remove a pixel from the buffer.*/
int
remove_pixel(const Pixel pix, Pose pose)
{
    int pdx;

    if (check_bounds(pose, WINSIZE))
        return (-1);

    pdx = TO_PDX(pose.row, pose.col);

    // Make sure not to strip higher ordered pixels of their spot.
    if (pix.z >= zord[pdx])
        zord[pdx] = 0; // Let anyone take this.

    if (oscore[pdx] > 0)
        oscore[pdx] -= 1; // Show one less occupant interested here.

    // No one wants this pixel, fill it for now.
    if (!oscore[pdx]) {
        set_pixel(*FILL_PIXEL, pose);
    }

    return (0);
}

/* Retrieve the pixel from the buffer. */
Pixel *
get_pixel(Pose p)
{
    int sdx, pdx;

    if (check_bounds(p, WINSIZE)) {
        return NULL;
    }

    sdx = TO_SDX(p.row, p.col);
    pdx = TO_PDX(p.row, p.col);


    Pixel *pixel = malloc(sizeof(Pixel));

    pixel->z = zord[pdx];

    memcpy(pixel->buf, &scr[sdx], PIXELLEN);
    return pixel;
}

/* Create a new Pixel. */
Pixel *
create_pixel(char c, PixelColor p, int z)
{
    Pixel *pixel = malloc(sizeof(Pixel));
    if (!pixel)
        return NULL;

    char tmp[PIXELLEN + 1]; // snprintf puts a \0 a huevo.
    snprintf(tmp, PIXELLEN + 1, PIXEL_FORMAT, p, BLACK, c);

    memcpy(pixel->buf, tmp, PIXELLEN); // Leave \0 out.
    pixel->z = z;

    return pixel;
}


/* 
 * Initialize the screen. 
 *
 * This MUST be called before any other method.
*/
void
init_scr(DrawOptions dopts)
{
    int i;

    // Set doptions.
    doptions = dopts;

    // Get Terminal Screensize.
    if (scrsize(&WINSIZE)) {
        return;
    }

    raw_mode(1);
    clrscr();
    setcsr_vis(0);

    // Each row should have PIXELLEN * number of colomns + newline
    rowlen = (PIXELLEN * WINSIZE.col) + 1;
    FILL = malloc(rowlen);

    FILL_PIXEL = create_pixel('.', WHITE, 0);
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

    debuglen = WINSIZE.row * WINSIZE.col;
    zord = calloc(debuglen, sizeof(int));
    oscore = calloc(debuglen, sizeof(int));

    draw();

    return;
}