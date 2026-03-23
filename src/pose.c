#include "pose.h"

Pose
add_pose(Pose x, Pose y)
{
    Pose t;
    t.row = x.row + y.row;
    t.col = x.col + y.col;

    return t;
}

int
check_bounds(Pose p, Pose b)
{
    if (p.row < 1 || p.row > b.row )
        return (-1);

    if (p.col < 1 || p.col > b.col)
        return (-1);

    return (0);
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