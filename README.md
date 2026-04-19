# tdraw

`tdraw` is a C11 terminal drawing playground with a small static library (`tdlib.a`) and a demo app (`tests/demo`).

It uses an in-memory screen buffer, ANSI escape sequences, and a threaded runtime that continuously moves and redraws models.

## Requirements

- macOS or Linux terminal with ANSI support
- `make`
- C compiler with C11 support (`cc`, `clang`, or `gcc`)

## Build And Run

Build default artifacts:

```bash
make
```

Run demo:

```bash
./tests/demo
```

Or build and run via Make target:

```bash
make run
```

## Project Layout

- `include/`: public headers
- `src/codes.c`: terminal/ANSI and tty helpers
- `src/draw.c`: screen buffer and pixel queue rendering
- `src/ipc.c`: Uses IPC to send a valid terminal for logging
- `src/logger.c`: logging used by tdraw
- `src/pose.c`: vector definitions
- `src/queue.c`: queue helpers
- `src/tdlib.c`: thin library facade
- `src/tdraw.c`: threaded runtime
- `tests/`: demo clients for testing tdraw

## Terminal Recovery

If your terminal state is left in a bad mode after interruption:

```bash
stty sane
printf '\033[?1049l\033[?25h'
```