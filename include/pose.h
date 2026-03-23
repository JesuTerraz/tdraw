#ifndef POSE_H
#define POSE_H

typedef struct pose {
    int row;
    int col;
} Pose;

Pose add_pose(Pose x, Pose y);
int check_bounds(Pose p, Pose b);
int check_rct_bounds(Pose p, Pose s, Pose b);

#endif