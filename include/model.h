#ifndef MODEL_H
#define MODEL_H

#include "draw.h"
#include "pose.h"

typedef struct model {
    Pose p;         // Current Position.
    Pose speed;     // Movement speed.
    Pixel *pixel;  // Pixels.
    int len;        // Num Pixels (limit 1 for now.)
} Model;

void draw_model(Model model);
int create_model(Model *model, char c, PixelColor p, int z);

#endif