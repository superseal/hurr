/*
     _ _      _        
  __| (_) ___| | _____ 
 / _` | |/ __| |/ / __|
| (_| | | (__|   <\__ \
 \__,_|_|\___|_|\_\___/
                  
*/

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

struct map {
	char ***map; /* Array of array of char pointers, or 2D matrix of strings */
	short height, width; /* Map dimensions */
} board;

struct position {
	short row, col;
} startpos; 

struct window {
	WINDOW *window; /* Pointer to ncurses window */
	int wheight, wwidth; /* Number of rows and columns in the window */
} win;

struct player {
	struct position pos; /* Current player position */
	char health, stamina;
	Inventory *itemlist; /* Player's own inventory */
};

/* Game objects list. _Do not_ modify this, the getters work with objects[] for
   their operations. This was made this way so getters would be like getprop(2)
   and not getprop(list, 2). */
// const Item *objects;

struct map mapopen();
void play();
void readkey(char input, struct position *pos);
void printmap(struct position startpos, struct position pos);
struct position scrollwindow(struct position startpos, struct position pos, char lastmove);
char checkcollisions(struct position pos, char input);
struct position moveobject(struct position pos, char input, char* newstring);
void shoot(struct position pos); 
WINDOW *createwin(int height, int width, int starty, int startx);
void closewin(WINDOW *win);
int gsleep(unsigned long usec);
int endgame(char exitcode);
void printmessage(char* message);
void hitobject(struct position pos, char input);
void clearmessages();

/*** main(), hurr ***/
int main() {
	/* ncurses initialization, receives single keypresses, doesn't print input to screen */
	initscr(); raw(); keypad(stdscr, TRUE); noecho(); 

	/* Hide cursor */
	curs_set(0);

	/* Load file to gamemap */
 	board = mapopen();

	/* Create game window */
	int wheight, wwidth;
	getmaxyx(stdscr, wheight, wwidth);
	/* Leave room for stats and message bar */
	wheight -= 2; 
	/* Print map and border starting from 1, 0 */
	WINDOW *mainwindow = createwin(wheight, wwidth, 1, 0);
	win = (struct window) {mainwindow, wheight, wwidth};

	/* Get read-only game objects list */
//	objects = getgameobjects();

	/* Start game */
	play();

	/* End game */
	endgame(0);
	return 0;
}	

/*** Read map from file ***/
struct map mapopen() {
	FILE *mapfile = fopen("map", "r");
	// if (mapfile == NULL) {return NULL;} 
	char charbuffer[3];
	short row, col, height, width;

	/* Count number of rows and columns in map */
	row = 0, col = 0;
	while (fgets(charbuffer, 3, mapfile)) {
		if (*charbuffer != '\n') {col++;}
		else {row++; width = col; col = 0;} 
	}
	height = row;

	/* Allocate memory for map */
	char ***map = malloc(height * sizeof(char**));
	for (row = 0; row < height; row++) {
		map[row] = malloc(width * sizeof(char*));
		for (col = 0; col < width; col++) {
			map[row][col] = malloc(3 * sizeof(char));
		}
	}
	
	/* Rewind fgets pointer to start of file */
	rewind(mapfile);

	/* Read 2 characters from file, assign to each map grid */
	row = 0, col = 0;
	while (fgets(charbuffer, 3, mapfile)) {
		if (*charbuffer != '\n') {map[row][col] = (char*) strdup(charbuffer); col++;}
		else {row++; col = 0;}
	}
	fclose(mapfile);
	struct map gamemap = {map, height, width};
	return gamemap;
}

/*** Start game ***/
void play() {
	/* Create player 1 */
	struct position pos1 = {1, 1}; /* Initial position on map for player 1 */
	Inventory *p1_items = list_create(); /* Create empty inventory for player 1 */
	struct player player1 = {pos1, 100, 100, p1_items};
	board.map[pos1.row][pos1.col] = PLAYER_STRING;

	/* Upper left window corner for printing */
	startpos = (struct position) {0, 0};

	char input, lastmove;
	wrefresh(win.window);
	while (1) { 
		startpos = scrollwindow(startpos, player1.pos, lastmove);
		printmap(startpos, player1.pos);
		input = getch();
		readkey(input, &(player1.pos)); 
		lastmove = input;
	}
}

/*** Execute a game command after getting user input ***/
void readkey(char input, struct position *pos) {
	switch (input) {
		case 'w': 
		case 'a':
		case 's':
		case 'd':
			if (!checkcollisions(*pos, input)) {
				*pos = moveobject(*pos, input, PLAYER_STRING);
			} 
			break;
		case 'e':
			shoot(*pos);
			break;
		/* (!) Debug */
		case 'r':
			list_insert(p1_items, 1, 0);
		case 'q':
			printmessage("Are you sure you want to end the game? [y/N]> ");
			char input = getch();
			if (input == 'y' || input == 'Y') {endgame(0);}
			clearmessages();
			break;
	}
}

/*** Scroll window if last move is 2 positions before touching the window borders ***/
struct position scrollwindow(struct position startpos, struct position pos, char lastmove) {
	/* pos.row - 2 == startrow - 2  ->  pos.row + 4 == startrow */
	short startrow = startpos.row, startcol = startpos.col;
	short row = pos.row, col = pos.col;
	switch (lastmove) {
		case 'w':
			if ((row - 1 == startrow) && (startrow > 0)) {
				--startrow;
			}; 
			break;
		case 'a': 
			if ((col - 1 == startcol) && (startcol > 0)) {
				--startcol;
			}; 
			break;
		case 's': 
			if ((row + 4 == startrow + win.wheight) && (startrow + win.wheight - 2 < board.height)) {
				++startrow;
			}; 
			break;
		case 'd': 
			if ((col + 3 == startcol + (win.wwidth/2)) && (startcol + (win.wwidth/2) - 1 < board.width)) {
				++startcol;
			}; 
			break;
	}
	struct position newstartpos = {startrow, startcol};
	return newstartpos;
}

/*** Print map to window ***/ 
void printmap(struct position startpos, struct position pos) {
	/* Substract 2 rows and cols (window borders) */
	short startrow = startpos.row, startcol = startpos.col;
	short height = (win.wheight - 2    < board.height ? win.wheight - 2    + startrow : board.height);
	short width  = ((win.wwidth/2) - 2 < board.width  ? (win.wwidth - 2)/2 + startcol : board.width);
	short row, col; /* Map row and column */
	short y, x; /* Coordinates passed to mvprintw function for printing on screen */
	for (row = startrow, y = 0; row < height; row++, y++) {
		for (col = startcol, x = 0; col < width; col++, x++) {
			/* Add 1 row and column (start from position 1, 1 of window win) */
            mvwprintw(win.window, (y + 1), ((2 * x) + 1), "%s", board.map[row][col]);
		}
	}
	/* Shows position in map */
	mvprintw(win.wheight, win.wwidth - 10, "[%2d,%2d]", pos.row, pos.col); 
	wrefresh(win.window);
}

/*** Check map boundaries and validate movements ***/
char checkcollisions(struct position pos, char input) {
	short row = pos.row, col = pos.col, height = board.height, width = board.width;
	short nextrow = row, nextcol = col;
	switch (input) {
		case 'w': --nextrow; break;
		case 'a': --nextcol; break;
		case 's': ++nextrow; break;
		case 'd': ++nextcol; break;
	}
 	return (nextrow < 0 || nextcol < 0 || 
			nextrow > (height - 1) || nextcol > (width - 1) ||
			strcmp(board.map[nextrow][nextcol], DEFAULT_STRING) != 0);
}

/*** Move object in grid **/
struct position moveobject(struct position pos, char input, char* newstring) {
	short row = pos.row, col = pos.col;
	board.map[row][col] = DEFAULT_STRING;
	switch (input) {
		case 'w': board.map[--row][col] = newstring; break;
		case 'a': board.map[row][--col] = newstring; break;
		case 's': board.map[++row][col] = newstring; break;
		case 'd': board.map[row][++col] = newstring; break;
	}
	struct position newpos = {row, col};
	return newpos;
}

/*** Shoots bullet ***/
void shoot(struct position pos) {
	struct position pos_f = {pos.row, pos.col};
	/* Check inventory for bullets */
	printmessage("Shoot> ");
	char input = getch();
	if (!checkcollisions(pos, input)) {
		switch (input) {
			case 'w': --(pos_f.row); break;
			case 'a': --(pos_f.col); break;
			case 's': ++(pos_f.row); break;
			case 'd': ++(pos_f.col); break;
		}
		board.map[pos_f.row][pos_f.col] = BULLET_STRING;
		printmap(startpos, pos);
		while (!checkcollisions(pos_f, input)) {
			pos_f = moveobject(pos_f, input, BULLET_STRING);
			/* Bullet animation */
			printmap(startpos, pos); gsleep(3448);
		}
		board.map[pos_f.row][pos_f.col] = " %";
		/* Hit animation */
		printmap(startpos, pos); gsleep(50000);
		board.map[pos_f.row][pos_f.col] = DEFAULT_STRING;
	}
	hitobject(pos_f, input);
	clearmessages();
}

/*** Calculate damage done by projectiles ***/
/* (!) Incomplete */
void hitobject(struct position pos, char input) {
	short row = pos.row, col = pos.col;
	switch (input) {
		case 'w': --row; break;
		case 'a': --col; break;
		case 's': ++row; break;
		case 'd': ++col; break;
	}
}

/*** Prints a message in messsage bar ***/
void printmessage(char* message) {
	clearmessages();
	mvprintw(0, 0, message); 
	wrefresh(win.window);
}

/*** Clears message bar ***/
void clearmessages() {
	int x;
	for (x = 0; x < win.wwidth; x++) {mvaddch(0, x, ' ');}
	wrefresh(win.window);
}

/*** Create ncurses window ***/
WINDOW *createwin(int height, int width, int starty, int startx) {
	WINDOW *win = newwin(height, width, starty, startx); 
	getch(); // Needs getch to create window border who knows why
	box(win, 0, 0);
	wrefresh(win);
	return win;
}

/*** Close ncurses window ***/
void closewin(WINDOW *win) {	
	wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(win);
	delwin(win);
}

/*** Sleeps for usec microseconds ***/
int gsleep(unsigned long usec) {
	struct timespec req = {0, 0};
	time_t sec = (int) (usec / 100000);
	usec -= (sec * 100000);
	req = (struct timespec) {sec, usec * 1000};
	while (nanosleep(&req, &req) == -1) {continue;}
	return 1;
}

/*** hurr ***/
int endgame(char exitcode) {
	refresh(); endwin(); exit(exitcode);
}
