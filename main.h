/* Needed for using POSIX functions like nanosleep and strdup */
#define _POSIX_C_SOURCE 200809L

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "inventory.h"
#include "itemlist.h"
#include <time.h>

#define PLAYER_STRING "**"
#define DEFAULT_STRING "  "
#define BULLET_STRING "^ "

/* Current board elements */
struct map {
	char ***map; /* Array of array of char pointers, or 2D matrix of strings */
	short height, width; /* Map dimensions */
} board;

/* Position elements */
struct position {
	short row, col;
} upperleft; /* Initial position */

/* Game window elements */
struct window {
	WINDOW *window; /* Pointer to ncurses window */
	int wheight, wwidth; /* Number of rows and columns in the window */
} messagewin, gamewin, statswin;

/* Player elements */
struct player {
	struct position pos; /* Current player position */
	char health, stamina;
	Inventory *itemlist; /* Player's own inventory */
};

/* Game objects list. _Do not_ modify this, the getters work with objects[] for
   their operations. This was made this way so getters would be like getprop(2)
   and not getprop(list, 2). */
// const Item *objects;

/* Read map from file */
struct map mapopen();

/* Start game */
void play();

/* Execute a game command after getting user input */
void readkey(char input, struct player *player1);

/* Scroll window if last move is 2 positions before touching the window borders */
struct position scrollwindow(struct position upperleft, struct position pos, char lastmove);

/* Print map to window */ 
void printmap(struct position upperleft, struct position pos);

/* Check map boundaries and validate movements */
char checkcollisions(struct position pos, char input);

/* Move object in map grid */
struct position moveobject(struct position pos, char input, char* newstring);

/* Shoots a single bullet */
void shoot(struct position pos); 

/* Calculate damage done by projectiles */
void hitobject(struct position pos, char input);

/* Shows current player inventory in a centered window */
void showinventory(struct player *player1);

/* Prints a message in messsage bar */
void printmessage(char* message);

/* Clears message bar */
void clearmessages();

/* Creates an ncurses window */
WINDOW *createwin(int height, int width, int starty, int startx);

/* Closes an ncurses window */
void closewin(WINDOW *win);

/* Sleeps for usec microseconds */
int gsleep(unsigned long usec);

/* Ends game, hurr */
int endgame(char exitcode);

