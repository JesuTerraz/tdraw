#ifndef MOVE_H
#define MOVE_H

#include "model.h"
#include "pose.h"

typedef enum {
    MOVE_NONE,
    MOVE_UP,
    MOVE_LEFT,
    MOVE_DOWN,
    MOVE_RIGHT,
} MoveCmd;

typedef struct move_pose {
    Pose *p;
    Pose offset;
} MovePose;

void move_buf(char *buf, int buflen, MovePose *mp);
void move_model(Model *mode, MoveCmd opt);

#endif