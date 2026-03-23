#include "input.h"

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