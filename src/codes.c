#include "codes.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/* Retrieve the position of the cursor. */
int 
getcsr(Pose *p) {
    char buf[32];
    int i = 0;

    if (write(STDOUT_FILENO, REQUEST_CRS, 4) != 4) {
        return (-1);
    }

    while (i < (int)sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }

    buf[i + 1] = '\0';

    if (sscanf(buf, CRS_POS_FORMAT, &p->row, &p->col) != 2)
        return (-1);

    return (0);
}

/* Set the cursor to a position.*/
int 
setcsr(Pose p) {
    char buf[32];
    int n;

    n = snprintf(buf, sizeof(buf), CRS_POS_FORMAT, p.row, p.col);
    if (n < 0 || n >= (int)sizeof(buf)) {
        return (-1);
    }

    if (write(STDOUT_FILENO, buf, n) != n) {
        return (-1);
    }

    return (0);
}

/* Set cursor visibility. */
int 
setcsr_vis(int set) {
    char *buf = set ? SHOW_CRS : HIDE_CRS;
    int buflen = strlen(buf);

    if (write(STDOUT_FILENO, buf, buflen) < 0) {
        return (-1);
    }

    return (0);
}

/* Set Terminal to Raw Mode (Echo and Canonical Mode)*/
void 
raw_mode(int set) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);

    if (set) {
        tty.c_lflag &= ~(ECHO | ICANON); // disable echo and canonical mode
        // tty.c_iflag &= ~(IXON | ICRNL);
    }

    if (!set)
        tty.c_lflag |= (ECHO | ICANON);   // Enable.

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tty);
}

/* Clear the screen */
int 
set_alt(int set) {
    char *buf = set ? ENTER_ALT_SCR : EXIT_ALT_SCR;
    int buflen = strlen(buf);

    if (write(STDOUT_FILENO, buf, buflen) < 0) {
        return (-1);
    }

    return (0);
}

int
clr_scr() {
    if (write(STDOUT_FILENO, CLR_SCROLL, strlen(CLR_SCROLL))) {
        return (-1);
    }

    if (write(STDOUT_FILENO, CLR_SCR, strlen(CLR_SCR))) {
        return (-1);
    }

    return (0);
}

/* Get screen size.*/
int 
scrsize(Pose *p) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0) {
        perror("ioctl");
        return (1);
    }

    p->row = ws.ws_row;
    p->col = ws.ws_col;
    return (0);
}