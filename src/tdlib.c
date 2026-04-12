#include "tdlib.h"

#include <stdlib.h>

int 
init()
{
    start_tdraw();
    return (0);
}

int 
shutdown()
{
    destory_scr();
    return (0);
}

int
submit_pixel(PixelOp *pop)
{
    if (!pop)
        return (-1);

    if (!pop->pixel)
        return (0);

    balance_routine(pop);

    return (0);
}

void
draw_model(Model *model)
{
    int i;
    PixelOp *op;

    if (!model)
        return;

    for (i = 0; i < model->len; i++)
    {
        if (!model->pixels[i])
            continue;

        op = malloc(sizeof(PixelOp));
        op->op = SET;
        op->pixel = model->pixels[i];

        submit_pixel(op);
    }
}

void
remove_model(Model *model)
{
    int i;
    PixelOp *op;

    if (!model)
        return;

    for (i = 0; i < model->len; i++)
    {
        if (!model->pixels[i])
            continue;

        op = malloc(sizeof(PixelOp));
        op->op = REMOVE;
        op->pixel = model->pixels[i];
        submit_pixel(op);
    }
}

void
move_model(Model *model, MoveCmd opt)
{
    Pose offset;
    Pose t;
    PixelOp *op;
    int i;

    if (!model) {
        return;
    }

    /* Find which movement is requested. */
    switch(opt) {
        case MOVE_NONE:
            offset.x = 0;
            offset.y = 0;
            break;
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
     * 
     * We'll take the model's pose for this. Any pixels that
     * offset out of bounds will simply not be shown.
    */
    t = add_pose(model->p.p, offset);
    if (check_bounds(t, WINSIZE)) {
        return;
    }

    /* 
     * Update position.
     *
     * Because submit takes a single mutable pixel. We can't
     * trust that either will occur first.
     * 
     * Best to ensure that the movement is atomic here.
    */
    model->p.p = t;
    for (i = 0; i < model->len; i++) {
        op = malloc(sizeof(PixelOp));
        op->pixel = model->pixels[i];
        op->opts.offset = offset;
        op->op = MOVE;

        submit_pixel(op);
    }
}

Model *
create_model(Pose3D pose, Pose speed, int num_pixels, PixelDef *pixels)
{
    Model *model;
    int i;

    model = malloc(sizeof(Model) + (num_pixels * sizeof(void *)));
    model->p = pose;
    model->speed = speed;
    model->len = num_pixels;

    for (i = 0; i < num_pixels; i++)
    {
        Pose3D actual_pose = add_pose3d(pose, pixels[i].offset);
        Pixel *pixel = create_pixel(pixels[i].c, pixels[i].colors, actual_pose);
        if (pixel == NULL) {
            return (NULL);
        }

        model->pixels[i] = pixel;
    }

    return (model);
}