#include "model.h"
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