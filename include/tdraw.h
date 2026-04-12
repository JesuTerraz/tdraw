#ifndef TDRAW_H
#define TDRAW_H

#include "draw.h"

typedef enum {
    NONE,
    SET,
    REMOVE,
    MOVE,
} OperationType;

typedef struct {
    Pose offset;
} OperationOpts;

typedef struct {
    OperationType op;
    Pixel *pixel;
    OperationOpts opts;
} PixelOp;

void start_tdraw();

void *balance_routine(PixelOp *pop);

#endif