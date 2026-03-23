#ifndef INPUT_H
#define INPUT_H

#include "move.h"

typedef enum {
    INPUT_NONE,
    INPUT_MOVE,
    INPUT_CMD,
} InputKind;

typedef enum {
    QUIT,
} InputCmd;

typedef struct {
    InputKind kind;
    union {
        MoveCmd move;
        InputCmd cmd;
    } value;
} Input;

Input parse_input(char c);

#endif