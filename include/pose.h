#ifndef POSE_H
#define POSE_H

typedef struct pose {
    int x;
    int y;
} Pose;

typedef struct pose3d {
    Pose p;     // 2D Representation.
    int z;
} Pose3D;

Pose add_pose(Pose x, Pose y);
int check_bounds(Pose p, Pose b);
int check_rct_bounds(Pose p, Pose s, Pose b);

Pose3D add_pose3d(Pose3D x, Pose3D y);
int check_bounds3d(Pose3D x, Pose y);

#endif