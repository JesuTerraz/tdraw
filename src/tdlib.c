#include "tdlib.h"

#include "tdraw.h"

int 
init()
{
    start_tdraw();
    return (0);
}

int 
shutdown()
{
    destory_scr();
    return (0);
}


int 
submit_model(Model *model)
{
    int i;

    if (!model)
    {
        return (-1);
    }

    if (!model->len)
    {
        return (0);
    }

    for (i = 0; i < model->len; i++)
    {
        balance_routine(model->pixels[i]);
    }

    return (0);
}

// int 
// move_model()
// {
//     return (0);
// }