# tdraw

`tdraw` is a small C project for drawing and moving colored characters in the terminal using ANSI escape codes and an in-memory screen buffer.

## Features

- Demonstrates basic terminal rendering, input handling, and model movement.
- Spawns one player (`X`, green) and many random entities (colored digits).
- Redraws the scene in a loop while reading keyboard input in raw mode.

## Requirements

- macOS or Linux terminal with ANSI escape sequence support
- `make`
- C compiler with C11 support (`cc`, `clang`, or `gcc`)

## Build

From the project root:

```bash
make
```

This builds the `tdraw` binary.

## Run

```bash
./tdraw
```

## Controls

- `w`: move up
- `a`: move left
- `s`: move down
- `d`: move right
- `q`: quit

## Make Targets

- `make` or `make all`: build
- `make clean`: remove object files
- `make fclean`: remove objects, `build/`, and binary
- `make re`: full rebuild

## Project Layout

- `src/main.c`: main loop, player/entity setup
- `src/draw.c`: screen buffer initialization and rendering
- `src/codes.c`: terminal control helpers (cursor, raw mode, clear screen)
- `src/input.c`: key parsing into input commands
- `src/move.c`: movement logic
- `src/model.c`: model creation and drawing
- `include/`: public headers and shared types

## Notes

- If your terminal state looks broken after exit, run:

```bash
stty sane
printf '\033[?1049l\033[?25h'
```