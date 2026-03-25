#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
draw_model(Model model)
{
    set_pixel(model.pixel);
}

int
create_model(Model *model, char c, Pose colors, Pose3D pose)
{
    Pixel *pixel = create_pixel(c, colors, pose);
    if (pixel == NULL) {
        return (-1);
    }

    model->pixel = pixel;
    return (0);
}