#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
draw_model(Model model)
{
    set_pixel(*model.pixel, model.p);
}

int
create_model(Model *model, char c, PixelColor p, int z)
{
    Pixel *pixel = create_pixel(c, p, z);
    if (pixel == NULL) {
        return (-1);
    }

    model->pixel = pixel;
    return (0);
}