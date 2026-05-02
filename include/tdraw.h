#ifndef TDRAW_H
#define TDRAW_H

#include "draw.h"
#include "model.h"
#include "queue.h"

typedef enum {
    NONE,
    SET,
    REMOVE,
    MOVE,
} OperationType;

typedef struct {
    Pose3D pose;
    Pose offset;
} OperationOpts;

typedef struct {
    OperationType op;
    OperationOpts opts;
    Queue pops;
} ModelOp;

typedef struct {
    OperationType op;
    OperationOpts opts;
    Pixel *pixel;
} PixelOp;

void start_tdraw();

void *balance_routine(Model *model, OperationType op);

#endif