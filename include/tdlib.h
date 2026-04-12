#ifndef TDLIB_H
#define TDLIB_H

#include "model.h"
#include "tdraw.h"


/*
 * Init all of tdraw.
*/
int init();

/*
 * Shutdown all of tdraw and restore.
*/
int shutdown();

/*
 * Submit a model to tdraw.
*/
int submit_pixel(PixelOp *pop);

/*
 * Draw a model.
*/
void draw_model(Model *model);

/*
 * Remvoe a model.
*/
void remove_model(Model *model);

/*
 * Move a specified model.
*/
void move_model(Model *model, MoveCmd opt);

/*
 * Create a model.
*/
Model *create_model(Pose3D pose, Pose speed, int num_pixels, PixelDef *pixels);

#endif