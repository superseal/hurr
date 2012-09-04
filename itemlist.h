#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* List all available item types */
typedef enum {
	car, firearm, bullet
} itemtype;

/* Bullet properties */
typedef struct bullettype {
	unsigned short velocity;
	unsigned short kineticenergy;
} Bullet;

/* Car properties */
typedef struct cartype {
	unsigned short maxspeed;
} Car;

/* Firearm properties */
typedef enum {
	handgun, shotgun, machinegun
} weapontype;

typedef struct firearmtype {
	weapontype weaptype;
	// set bulletid;
	unsigned char capacity;
} Firearm;

/* General item properties */
typedef struct gameitem {
	itemtype type;
	char* name;
	unsigned short bulk; 
	unsigned short weight;
	union {
		Bullet bul;
		Car car;
		Firearm fir;
	} properties;
} Item;

/* Using these is probably safer than a generic newitem() function with a
   variable argument list. Or not. I'm fully aware this is bloat but it's
   enterprise grade scalable bloat. */
/*** Item creators ***/
Item newbullet(char* name, unsigned short bulk, unsigned short weight, unsigned short velocity, unsigned short kineticenergy);
Item newcar(char* name, unsigned short bulk, unsigned short weight, unsigned short maxspeed);
Item newfirearm(char* name, unsigned short bulk, unsigned short weight, weapontype weaptype, unsigned char capacity);

/* Game objects list. _Do not_ modify this, the getter functions use objects[]
   for their operations. It was made this way so getters would be like
   getprop(list, 2) instead of getprop(list, 2). */
const Item *objects;


/*** Returns a list with all the game objects ***/
Item *getgameobjects();

/*** Generic getter for all items, returns name ***/
char *getname(short item_id);


/*** Bullet getters ***/
unsigned short getvelocity(short bullet_id);
unsigned short getkineticenergy(short bullet_id);

/*** Car getters ***/
unsigned short getmaxspeed(short car_id);

/*** Firearm getters ***/
weapontype getweaptype(short firearm_id);
unsigned char getcapacity(short firearm_id);
