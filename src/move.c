#include "move.h"

#include "draw.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
move_pixel(Pixel *pixel, MovePose *mp)
{
    if (!pixel || !mp) {
        return;
    }

    Pose t = add_pose(*mp->p, mp->offset);

    if (check_bounds(t, WINSIZE)) {
        return;
    }

    /* Erase where the pixel was. */
    remove_pixel(pixel, *(mp->p));

    /* Fill where pixel is now*/
    mp->p->row = t.row;
    mp->p->col = t.col;
    set_pixel(pixel, *(mp->p));
}

/*
 * Only suports moving Models that define 1 line.
*/
void
move_model(Model *model, MoveCmd opt)
{
    MovePose mp;

    if (!model) {
        return;
    }

    /* Find which movement is requested. */
    switch(opt) {
        case MOVE_UP:
            mp.offset.row = -1 * model->speed.row;
            mp.offset.col = 0;
            break;
        case MOVE_DOWN:
            mp.offset.row = model->speed.row;
            mp.offset.col = 0;
            break;
        case MOVE_RIGHT:
            mp.offset.row = 0;
            mp.offset.col = model->speed.col;
            break;
        case MOVE_LEFT:
            mp.offset.row = 0;
            mp.offset.col = -1 * model->speed.col;
            break;
        default:
            return;
    }

    /* 
     * Do some checks to make sure movement won't leave 
     * the model outside of the window.
    */
    // Done by the move_buf() call, no need to check here.

    /* Move the model.*/
    mp.p = &model->p;
    move_pixel(model->pixel, &mp);
}