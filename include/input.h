#ifndef INPUT_H
#define INPUT_H

#include "model.h"

typedef enum {
    INPUT_NONE,
    INPUT_MOVE,
    INPUT_CMD,
    INPUT_CMDLINE,
} InputKind;

typedef enum {
    QUIT,
} InputCmd;

typedef enum {
    NODEBUG,
    PRINT,          // Print logs to terminal.
} DebugOptions;

typedef struct {
    InputKind kind;
    union {
        MoveCmd move;
        InputCmd cmd;
    } value;
} Input;

typedef struct {
    DrawOptions dopts;
    DebugOptions  debug;
} Config;

extern Config CONFIG;

Input parse_input(char c);
Config parse_cmdline(int argc, char **argv);

#endif