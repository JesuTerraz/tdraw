#include "input.h"

#include <string.h>

Config CONFIG =  { 
    DRAW,
    NODEBUG 
};

Input
parse_input(char c)
{
    Input input;
    switch (c) {
        case 'w': 
            input.kind = INPUT_MOVE;
            input.value.move = MOVE_UP;
            break;
        case 'a': 
            input.kind = INPUT_MOVE;
            input.value.move = MOVE_LEFT;
            break;
        case 's':
            input.kind = INPUT_MOVE;
            input.value.move = MOVE_DOWN;
            break;
        case 'd':
            input.kind = INPUT_MOVE;
            input.value.move = MOVE_RIGHT;
            break;
        case 'q':
            input.kind = INPUT_CMD;
            input.value.cmd = QUIT;
            break;
        default:
            input.kind = INPUT_NONE;
    }

    return input;
}

Config
parse_cmdline(int argc, char **argv)
{
    int i;
    if (argc <= 1) {
        return CONFIG;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o")) {
            CONFIG.dopts = OCCUPY;
        }

        if (strcmp(argv[i], "-z")) {
            CONFIG.dopts = ORDER;
        }

        if (strcmp(argv[i], "-v")) {
            CONFIG.debug = PRINT;
        }
    }

    return CONFIG;
}