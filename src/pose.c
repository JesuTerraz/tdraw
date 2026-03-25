#include "pose.h"

Pose
add_pose(Pose a, Pose b)
{
    Pose t;
    t.x = a.x + b.x;
    t.y = a.y + b.y;

    return (t);
}

Pose3D
add_pose3d(Pose3D a, Pose3D b)
{
    Pose3D t;
    Pose t2D = add_pose(a.p, b.p);

    t.p = t2D;
    t.z = a.z + b.z;

    return (t);
}

int
check_bounds(Pose p, Pose b)
{
    if (p.x < 1 || p.x > b.x )
        return (-1);

    if (p.y < 1 || p.y > b.y)
        return (-1);

    return (0);
}

int
check_bounds3d(Pose3D p, Pose b)
{
    return (check_bounds(p.p, b));
}

int
check_rct_bounds(Pose p, Pose s, Pose b)
{
    /* Check if new position is bad.*/
    if (check_bounds(p, b))
        return (-1);

    /* Check if the model will fit.*/
    Pose sp = add_pose(p, s);
    if (check_bounds(sp, b))
        return (-1);

    return (0);
}