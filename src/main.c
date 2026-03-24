#include "move.h"
#include "model.h"
#include "draw.h"
#include "pose.h"
#include "input.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_ENTITIES 10

Model
create_player_model(void)
{
    Model m;

    Pose p = { 1, 1 };
    Pose v = { 1, 1 };

    m.p = p;
    m.speed = v;

    create_model(&m, 'X', GREEN, 2);

    return (m);
}

Model
create_entity_model(void)
{
    Model m;

    int x = (rand() % WINSIZE.row) + 1;
    int y = (rand() % WINSIZE.col) + 1;

    Pose p = { x, y };
    Pose v = { 1, 1 };

    m.p = p;
    m.speed = v;

    char c = '0' + (rand() % 10);
    PixelColor pc = (rand() % WHITE);
    create_model(&m, c, pc, 1);

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

    Model player = create_player_model();

    Model entities[NUM_ENTITIES];
    for (i = 0; i < NUM_ENTITIES; i++) {
        entities[i] = create_entity_model();
        draw_model(entities[i]);
    }

    draw_model(player);

    draw();

    while(read(STDIN_FILENO, &c, 1) == 1) {
        Input input = parse_input(c);

        if (input.kind == INPUT_MOVE) {
            move_model(&player, input.value.move);
        }

        if (input.kind == INPUT_CMD) {
            // Quit
            if (input.value.cmd == QUIT)
                break;
        }
        
        for (i = 0; i < NUM_ENTITIES; i++) {
            play_entity(&entities[i]);
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

    init_scr(CONFIG.dopts);
    srand(time(NULL));

    player_loop();

    destory_scr();

    return (0);
}