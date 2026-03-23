#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
draw_model(Model model)
{
    write_pixel(*model.pixel, model.p);
}

int
create_model(Model *model, char c, PixelColor p)
{
    Pixel *pixel = get_pixel(c, p);
    if (pixel == NULL) {
        return (-1);
    }

    model->pixel = pixel;
    return (0);
}