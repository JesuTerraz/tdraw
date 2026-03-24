#ifndef CODES_H
#define CODES_H

#include "pose.h"

/*
 * Cursor Movement
 */
#define MOVE_CRS_HOME       "\033[H"
#define MOVE_CRS_UP         "\033[A"
#define MOVE_CRS_DOWN       "\033[B"
#define MOVE_CRS_RIGHT      "\033[C"
#define MOVE_CRS_LEFT       "\033[D"

/* Takes row x col. */
#define CRS_POS_FORMAT  "\033[%d;%dH"
/* Move n steps in a direction.*/
#define MOVE_N_FORMAT   "\033[%dA/B/C/D"

#define SAVE_CRS        "\033[s"
#define RESTORE_CRS     "\033[u"
/* Write this code to request cursor position. */
#define REQUEST_CRS     "\033[6n"

#define HIDE_CRS        "\033[?25l"
#define SHOW_CRS        "\033[?25h"

/*
 * Screen Management
 */
#define CLR_SCR         "\033[2J"
#define CLR_SCROLL      "\033[3J"
#define ENTER_ALT_SCR   "\033[?1049h"
#define EXIT_ALT_SCR    "\033[?1049l"
#define CLR_LINE_FC     "\033[K"
#define CLR_LINE        "\033[2K"

/* 
 * Text Colors
 */
#define RESET_ATTR      "\033[0m"
#define TEXT_BOLD       "\033[1m"

/* Only use integers 0 - 7. 0 (black) - 7 (white)*/
#define SET_FG_FORMAT   "\033[3%dm"
/* Only use integers 0 - 7. 0 (black) - 7 (white)*/
#define SET_BG_FORMAT   "\033[4%dm" // 
/* Only use integers 0 - 7.*/
#define SET_BFG_FORMAT  "\033[9%dm" //

/*
 * Functions
 */

void draw(void);
int getcsr(Pose *p);
int setcsr(Pose p);
int setcsr_vis(int set);
void raw_mode(int set);
int clrscr(void);
int scrsize(Pose *ws);

#endif