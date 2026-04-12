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
    int len;            // Num Pixels
    Pixel *pixels[0];   // Pixels.
} Model;

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

#endif