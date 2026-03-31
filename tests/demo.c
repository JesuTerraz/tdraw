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

    Pose3D p = { { 1, 1 }, 3};
    Pose v = { 1, 1 };
    Pose c = { RED, WHITE };
    PixelDef pixeldefs[1] = {
        {'X', c, {{ 0, 0 }, 0}},
    };

    m = create_model(p, v, 1, pixeldefs);

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
    PixelDef pixeldefs[1] = {
        {c, colors, {{0, 0}, 0}}
    };


    m = create_model(p, v, 1, pixeldefs);

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
    char c;
    int i;

    Model *player = create_player_model();

    if (!player) {
        return;
    }

    Model *(entities[NUM_ENTITIES]);
    for (i = 0; i < NUM_ENTITIES; i++) {
        entities[i] = create_entity_model();
        draw_model(entities[i]);
    }

    draw_model(player);

    draw();

    while(read(STDIN_FILENO, &c, 1) == 1) {
        Input input = parse_input(c);

        if (input.kind == INPUT_MOVE) {
            move_model(player, input.value.move);
        }

        if (input.kind == INPUT_CMD) {
            // Quit
            if (input.value.cmd == QUIT)
                break;
        }
        
        for (i = 0; i < NUM_ENTITIES; i++) {
            play_entity(entities[i]);
        }

        draw();
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

    destory_scr();

    return (0);
}