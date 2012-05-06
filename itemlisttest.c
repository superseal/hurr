/* 
 * Lots of 
 *  _____ _   _ _____ _____ ____  ____  ____  ___ ____  _____ 
 * | ____| \ | |_   _| ____|  _ \|  _ \|  _ \|_ _/ ___|| ____|
 * |  _| |  \| | | | |  _| | |_) | |_) | |_) || |\___ \|  _|  
 * | |___| |\  | | | | |___|  _ <|  __/|  _ < | | ___) | |___ 
 * |_____|_| \_| |_| |_____|_| \_\_|   |_| \_\___|____/|_____|
 *
 * GRADE ENCAPSULATION incoming. This makes read operations safer and
 * makes code look a bit cleaner.
 *
 * You were warned beforehand.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* List all available item types */
typedef enum {
	car, gun, bullet
} itemtype;

/*** Bullet properties ***/
typedef struct bullettype {
	unsigned short velocity;
	unsigned short kineticenergy;
} Bullet;

/*** Car properties ***/
typedef struct cartype {
	unsigned short maxspeed;
} Car;

/*** Firearm properties ***/
typedef enum {
	handgun, shotgun, machinegun
} weapontype;

typedef struct firearmtype {
	weapontype type;
	// set bulletid;
	unsigned char capacity;
} Firearm;

/*** General item properties ***/
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
Item newbullet(char* name, unsigned short bulk, unsigned short weight, unsigned short velocity, unsigned short kineticenergy);
Item newcar(char* name, unsigned short bulk, unsigned short weight, unsigned short maxspeed);
Item newfirearm(char name, unsigned short bulk, unsigned short weight, weapontype type, unsigned char capacity);

Item *getgameobjects();

/* Bullet getters */
unsigned short getvelocity(short bullet_id);
unsigned short getkineticenergy(short bullet_id);
/* Car getters */
unsigned short getmaxspeed(short car_id);
/* Firearm getters */
unsigned char getcapacity(short firearm_id);

const Item *list;

int main() {
	list = getgameobjects();
	printf("%d \n", getvelocity(2));
	free((Item*) list);
	return 0;
}

Item newbullet(char* name, unsigned short bulk, unsigned short weight, unsigned short velocity, unsigned short kineticenergy) {
	return (Item) {bullet, name, bulk, weight, .properties.bul = {velocity, kineticenergy}};
}

unsigned short getvelocity(short bullet_id) {
	assert(list[bullet_id].type == bullet); return list[bullet_id].properties.bul.velocity;
}

unsigned short getkineticenergy(short bullet_id) {
	assert(list[bullet_id].type == bullet); return list[bullet_id].properties.bul.kineticenergy;
}

Item *getgameobjects() {
	Item *objects = malloc(50 * sizeof(Item));
	objects[0] = (Item) {gun, "a gun", 500, 5000, .properties.fir = {handgun, 10}};
	objects[1] = (Item) {car, "a car", 3500, 4000, .properties.car = {200}};
 	objects[2] = newbullet("a bullet", 200, 3000, 300, 5000);
	return objects;
}


