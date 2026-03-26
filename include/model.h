#ifndef MODEL_H
#define MODEL_H

#include "draw.h"
#include "pose.h"

typedef struct {
    char c;
    Pose colors;
    Pose3D offset;  // Pixel offset from Model Pose.
} PixelDef;

typedef struct model {
    Pose3D p;           // Current Position.
    Pose speed;         // Movement speed.
    int len;            // Num Pixels (limit 1 for now.)
    Pixel *pixels[0];   // Pixels.
} Model;


void draw_model(Model *model);
Model *create_model(Pose3D pose, Pose speed, int num_pixels, PixelDef *pixels);

#endif