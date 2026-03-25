#include "move.h"

#include "draw.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
move_pixel(Pixel *pixel, Pose offset)
{
    if (!pixel) {
        return;
    }

    Pose t = add_pose(pixel->pose.p, offset);

    if (check_bounds(t, WINSIZE)) {
        return;
    }

    /* Erase where the pixel was. */
    remove_pixel(pixel);

    /* Fill where pixel is now*/
    pixel->pose.p = t;
    set_pixel(pixel);
}

/*
 * Only suports moving Models that define 1 line.
*/
void
move_model(Model *model, MoveCmd opt)
{
    Pose offset;

    if (!model) {
        return;
    }

    /* Find which movement is requested. */
    switch(opt) {
        case MOVE_UP:
            offset.x = -1 * model->speed.x;
            offset.y = 0;
            break;
        case MOVE_DOWN:
            offset.x = model->speed.x;
            offset.y = 0;
            break;
        case MOVE_RIGHT:
            offset.x = 0;
            offset.y = model->speed.y;
            break;
        case MOVE_LEFT:
            offset.x = 0;
            offset.y = -1 * model->speed.y;
            break;
        default:
            return;
    }

    /* 
     * Do some checks to make sure movement won't leave 
     * the model outside of the window.
    */

    /* Move the model.*/
    move_pixel(model->pixel, offset);
}