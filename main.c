/*
     _ _      _        
  __| (_) ___| | _____ 
 / _` | |/ __| |/ / __|
| (_| | | (__|   <\__ \
 \__,_|_|\___|_|\_\___/
                  
*/

#include "main.h"

int main() {
	initscr(); /* Initialize ncurses */
	raw(); /* Receive single keypresses */
	keypad(stdscr, TRUE); /* Enable F keys and numeric keypad */
	noecho(); /* Don't print input to screen */
	curs_set(0); /* Hide cursor */

	/* Load file to gamemap */
 	board = mapopen();

	/* Create message, game and stats windows */
	int wheight, wwidth;
	getmaxyx(stdscr, wheight, wwidth);
	/* Leave room for stats and message bar */
	wheight -= 2; 
	WINDOW *messagewindow = createwin(1, wwidth, 0, 0);
	/* Print map and border starting from (y, x) = (1, 0) */
	WINDOW *gamewindow = createwin(wheight, wwidth, 1, 0);
	WINDOW *statswindow = createwin(1, wwidth, wheight, 0);
	
	messagewin = (struct window) {messagewindow, wheight, wwidth};
	gamewin = (struct window) {gamewindow, wheight, wwidth};
	statswin = (struct window) {statswindow, wheight, wwidth};

	/* Get read-only game objects list */
	objects = getgameobjects();

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
	upperleft = (struct position) {0, 0};

	char input, lastmove;
	wrefresh(gamewin.window);
	while (1) { 
		upperleft = scrollwindow(upperleft, player1.pos, lastmove);
		printmap(upperleft, player1.pos);
		input = wgetch(gamewin.window);
		readkey(input, &player1); 
		lastmove = input;
	}
}

/*** Execute a game command after getting user input ***/
void readkey(char input, struct player *player1) {
	/* [i] You're taking a pointer to a player struct here. Be careful. */
	/* This should take the address of the position struct, which is a member
	   of the player struct. But we have a pointer to a player struct here,
	   hence the ->. The pointer address is used so the value of the position can be
	   updated from the inside of this function. */
	struct position *pos = &(player1->pos); 
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
			showinventory(player1);
			list_insert(player1->itemlist, 0, 1, 0);
			break;
		case 'q':
			printmessage("Are you sure you want to end the game? [y/N]> ");
			char input = wgetch(messagewin.window);
			if (input == 'y' || input == 'Y') {endgame(0);}
			clearmessages();
			break;
	}
}

/*** Scroll window if last move is 2 positions before touching the window borders ***/
struct position scrollwindow(struct position upperleft, struct position pos, char lastmove) {
	/* pos.row - 2 == startrow - 2  ->  pos.row + 4 == startrow */
	short startrow = upperleft.row, startcol = upperleft.col;
	short row = pos.row, col = pos.col;
	char scr;
	switch (lastmove) {
		case 'w':
			scr = (row - 1 == startrow) && (startrow > 0);
			if (scr) {--startrow;}
			break;
		case 'a': 
			scr = (col - 1 == startcol) && (startcol > 0);
			if (scr) {--startcol;}
			break;
		case 's': 
			scr = (row + 4 == startrow + gamewin.wheight) && (startrow + gamewin.wheight - 2 < board.height);
			if (scr) {++startrow;}
			break;
		case 'd': 
			scr = (col + 3 == startcol + (gamewin.wwidth/2)) && (startcol + (gamewin.wwidth/2) - 1 < board.width);
			if (scr) {++startcol;} 
			break;
	}
	struct position newupperleft = {startrow, startcol};
	return newupperleft;
}

/*** Print map to window ***/ 
void printmap(struct position upperleft, struct position pos) {
	/* Substract 2 rows and cols (window borders) */
	short startrow = upperleft.row, startcol = upperleft.col;
	short height = (gamewin.wheight - 2    < board.height ? gamewin.wheight - 2    + startrow : board.height);
	short width  = ((gamewin.wwidth/2) - 2 < board.width  ? (gamewin.wwidth - 2)/2 + startcol : board.width);
	short row, col; /* Map row and column */
	short y, x; /* Coordinates passed to mvprintw function for printing on screen */
	for (row = startrow, y = 0; row < height; row++, y++) {
		for (col = startcol, x = 0; col < width; col++, x++) {
			/* Add 1 row and column (start from position 1, 1 of window gamewin) */
            mvwprintw(gamewin.window, (y + 1), ((2 * x) + 1), "%s", board.map[row][col]);
		}
	}
	/* Shows position in map */
	mvwprintw(gamewin.window, gamewin.wheight - 1, gamewin.wwidth - 10, "[%2d,%2d]", pos.row, pos.col); 
	wrefresh(gamewin.window);
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

/*** Move object in map grid ***/
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

/*** Shoots a single bullet ***/
void shoot(struct position pos) {
	struct position pos_f = {pos.row, pos.col};
	/* Check inventory for bullets */
	printmessage("Shoot> ");
	char input = wgetch(gamewin.window);
	if (!checkcollisions(pos, input)) {
		switch (input) {
			case 'w': --(pos_f.row); break;
			case 'a': --(pos_f.col); break;
			case 's': ++(pos_f.row); break;
			case 'd': ++(pos_f.col); break;
		}
		board.map[pos_f.row][pos_f.col] = BULLET_STRING;
		printmap(upperleft, pos);
		while (!checkcollisions(pos_f, input)) {
			pos_f = moveobject(pos_f, input, BULLET_STRING);
			/* Bullet animation */
			printmap(upperleft, pos); gsleep(3448);
		}
		board.map[pos_f.row][pos_f.col] = "``";
		/* Hit animation */
		printmap(upperleft, pos); gsleep(50000);
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

/*** Shows current player inventory in a centered window ***/
void showinventory(struct player *player1) {
	/* Add 2 for borders */ 
	int starty = (gamewin.wheight - 2*(10 + 2)) / 2;
	int startx = (gamewin.wwidth  - (54 + 2)) / 2;
	int width  = 54 + 2;
	int height = gamewin.wheight - 12;
	WINDOW *inventorywindow = createwin(height, width, starty, startx);
	wrefresh(inventorywindow);
	wgetch(inventorywindow);
	closewin(inventorywindow);
}

/*** Prints a message in messsage bar ***/
void printmessage(char* message) {
	clearmessages();
	mvwprintw(messagewin.window, 0, 0, message); 
	wrefresh(messagewin.window);
}

/*** Clears message bar ***/
void clearmessages() {
	int x;
	for (x = 0; x < messagewin.wwidth; x++) {mvwaddch(messagewin.window, 0, x, ' ');}
	wrefresh(messagewin.window);
}

/*** Creates an ncurses window ***/
WINDOW *createwin(int height, int width, int starty, int startx) {
	WINDOW *win = newwin(height, width, starty, startx); 
	box(win, 0, 0);
	wrefresh(win);
	refresh();
	return win;
}

/*** Closes an ncurses window ***/
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

/*** Ends game, hurr ***/
int endgame(char exitcode) {
	refresh(); endwin(); exit(exitcode);
}
