#include "move.h"
#include "model.h"
#include "draw.h"
#include "pose.h"
#include "input.h"
#include "logger.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_ENTITIES 10000

static Logger *logger;

Model
create_player_model(void)
{
    Model m;

    Pose3D p = { { 1, 1 }, 3};
    Pose v = { 1, 1 };
    Pose c = { RED, WHITE };

    m.p = p;
    m.speed = v;

    create_model(&m, 'X', c, p);

    return (m);
}

Model
create_entity_model(void)
{
    Model m;

    int x = (rand() % WINSIZE.x) + 1;
    int y = (rand() % WINSIZE.y) + 1;

    Pose3D p = { { x, y }, 1 };
    Pose v = { 1, 1 };

    m.p = p;
    m.speed = v;

    char c = '0' + (rand() % 10);
    Pose colors = { (rand() % WHITE), (rand() % WHITE) };
    create_model(&m, c, colors, p);

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

        debug(logger, "Received input");

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

    logger = init_logger("main");

    init_scr(CONFIG.dopts);
    srand(time(NULL));

    player_loop();

    destory_scr();

    close_logger(logger);

    return (0);
}