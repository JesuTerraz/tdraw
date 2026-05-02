#include "tdlib.h"

#include "logger.h"

#include <stdlib.h>

int 
init()
{
    if (init_logger())
    {
        printf("Could not init logger\n");
        return (-1);
    }

    set_level(LOG_WARN);

    start_tdraw();

    return (0);
}

int 
shutdown()
{
    destory_scr();
    return (0);
}

void
draw_model(Model *model)
{
    if (!model)
        return;

    balance_routine(model, SET);
}

void
remove_model(Model *model)
{
    if (!model)
        return;

    balance_routine(model, REMOVE);
}

void
move_model(Model *model, MoveCmd opt)
{
    Pose offset;
    Pose t;
    // PixelOp *op;
    // int i;

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
     * We want Pixel to now be immutable. So non atomic operation 
     * should be okay here...
    */
    remove_model(model);

    model->p.p = t;

    draw_model(model);
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
        /*
         * Pixels will now have offset as their pose.
         * 
         * Actual pose will have to be determined at submit time and provided by
         * OperationOptions
        */
        Pixel *pixel = create_pixel(pixels[i].c, pixels[i].colors, pixels[i].offset);
        if (pixel == NULL) {
            return (NULL);
        }

        model->pixels[i] = pixel;
    }

    return (model);
}