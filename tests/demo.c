#include "tdlib.h"

#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_ENTITIES 10000


Model *
create_player_model(void)
{
    Model *m;

    Pose3D p = { { 20, 20 }, 3};
    Pose v = { 1, 1 };
    Pose c = { WHITE, BLACK };
    PixelDef pixeldefs[9] = {
        {'X', c, {{ 0, 0 }, 0}},
        {'X', c, {{ 0, 1 }, 0}},
        {'X', c, {{ 0, 2 }, 0}},
        {'X', c, {{ 1, 1 }, 0}},
        {'X', c, {{ -1, 1 }, 0}},
        {' ', c, {{ 1, 0 }, 0}},
        {' ', c, {{ -1, 0 }, 0}},
        {' ', c, {{ 1, 2 }, 0}},
        {' ', c, {{ -1, 2 }, 0}},
    };

    m = create_model(p, v, 9, pixeldefs);

    return (m);
}

Model *
create_entity_model(void)
{
    Model *m;

    int x = (rand() % WINSIZE.x) + 1;
    int y = (rand() % WINSIZE.y) + 1;

    Pose3D p = { { x, y }, 1 };
    Pose v = { 1, 1 };
    Pose colors = { (rand() % WHITE), (rand() % WHITE) };
    char c = '0' + (rand() % 10);
    PixelDef pixeldefs[5] = {
        {c, colors, {{ 0, 0 }, 0}},
        {c, colors, {{ 0, 1 }, 0}},
        {c, colors, {{ 0, 2 }, 0}},
        {c, colors, {{ 1, 1 }, 0}},
        {c, colors, {{ -1, 1 }, 0}},
    };


    m = create_model(p, v, 5, pixeldefs);

    return (m);
}

void
play_entity(Model *entity)
{
    MoveCmd r = (rand() % MOVE_RIGHT) + 1;
    move_model(entity, r);
}

void 
player_loop(void)
{
    // char c;
    int i;

    Model *player = create_player_model();

    if (!player) {
        return;
    }

    submit_model(player);

    Model *(entities[NUM_ENTITIES]);
    for (i = 0; i < NUM_ENTITIES; i++) {
        entities[i] = create_entity_model();
        if (!entities[i])
            continue;

        submit_model(entities[i]);
    }

    while(1) {
        // play_entity(entities[0]);
        usleep(100000);
    }
}


int 
main(int argc, char **argv)
{
    if (argc > 1) {
        parse_cmdline(argc, argv);
    }

    init();

    srand(time(NULL));

    player_loop();

    shutdown();

    return (0);
}