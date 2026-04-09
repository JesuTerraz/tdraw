#include "model.h"

#include "draw.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
draw_model(Model *model)
{
    int i;

    if (!model)
    {
        return;
    }

    for (i = 0; i < model->len; i++)
    {
        if (!model->pixels[i]) {
            continue;
        }

        set_pixel(model->pixels[i]);
    }
}

void
move_pixel(Pixel *pixel, Pose offset)
{
    if (!pixel) {
        return;
    }

    /* Erase where the pixel was. */
    remove_pixel(pixel);

    Pose t = add_pose(pixel->pose.p, offset);
    pixel->pose.p = t;

    /* Fill where pixel is now*/
    set_pixel(pixel);
}

void
move_model(Model *model, MoveCmd opt)
{
    Pose offset;
    int i;

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
     * 
     * We'll take the model's pose for this. Any pixels that
     * offset out of bounds will simply not be shown.
    */
    Pose t = add_pose(model->p.p, offset);
    if (check_bounds(t, WINSIZE)) {
        return;
    }

    /* Move the model.*/
    for (i = 0; i < model->len; i++) {
        move_pixel(model->pixels[i], offset);
    }

    model->p.p = t;
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