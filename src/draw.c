#include "draw.h"

#include "codes.h"
#include "queue.h"

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
 * Represents a queue of pixels that want to be drawn to a position.
 *  The first pixel an any queue is always written.
 *  If a pixel is the first by the time draw() is called, this pixel is draw.
 * 
 *  IMPORTANT: Use PDX to index.
*/
static Queue *pqueues;

static DrawOptions doptions;

/*
* A Full row of the Screen Buffer.
* Use FDX to index it.
*/
char *FILL;
Pixel *FILL_PIXEL;
static QNode *FILL_QNODE;

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
set_pixel(const Pixel *pix, Pose pose)
{
    int pdx;
    QNode *node;

    if (check_bounds(pose, WINSIZE))
        return (-1);

    if ((node = create_node(pix, pix->z)) == NULL)
        return (-1);

    pdx = TO_PDX(pose.row, pose.col);
    if (!queue_append(&pqueues[pdx], node)) {
        return (0);
    }

    // Queue head has changed.
    const Pixel *head_pix = pqueues[pdx].head->val;
    return (write_point(head_pix->buf, PIXELLEN, pose));
}

/* Remove a pixel from the buffer.*/
int
remove_pixel(const Pixel *pix, Pose pose)
{
    int pdx;
    QNode *node;

    if (check_bounds(pose, WINSIZE))
        return (-1);

    if ((node = create_node(pix, pix->z)) == NULL)
        return (-1);

    pdx = TO_PDX(pose.row, pose.col);
    if(!queue_remove(&pqueues[pdx], *node)) {
        free(node);
        return (0);
    }

    free(node);

    // Queue head has changed.
    const Pixel *head_pix = pqueues[pdx].head->val;
    return (write_point(head_pix->buf, PIXELLEN, pose));
}

/* Retrieve the pixel from the buffer. */
Pixel *
get_pixel(Pose p)
{
    int pdx;

    if (check_bounds(p, WINSIZE)) {
        return NULL;
    }

    pdx = TO_PDX(p.row, p.col);

    // If queue has been working, whatever pixel is at head is the same
    // as what's in the buffer.
    Pixel *pixel = (Pixel *) pqueues[pdx].head->val;
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

    set_alt(1);
    clr_scr();
    raw_mode(1);
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

    pqueues = calloc(WINSIZE.row * WINSIZE.col, sizeof(Queue));

    // Fill pqueues by setting FILL_PIXEL as the first node.
    FILL_QNODE = create_node(FILL_PIXEL, 0);
    for (i = 0; i < WINSIZE.row * WINSIZE.col; i++) {
        queue_append(&pqueues[i], FILL_QNODE);
    }

    draw();

    return;
}

void
destory_scr()
{
    free(FILL);
    free(FILL_PIXEL);
    free(scr);
    free(pqueues);

    setcsr_vis(1);
    raw_mode(0);
    set_alt(0);
}