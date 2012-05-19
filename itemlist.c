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

#include "itemlist.h"

/*** Item creators ***/
Item newbullet(char* name, unsigned short bulk, unsigned short weight, unsigned short velocity, unsigned short kineticenergy) {
	return (Item) {bullet, name, bulk, weight, .properties.bul = {velocity, kineticenergy}};
}

Item newcar(char* name, unsigned short bulk, unsigned short weight, unsigned short maxspeed) {
	return (Item) {car, name, bulk, weight, .properties.car = {maxspeed}};
}

Item newfirearm(char* name, unsigned short bulk, unsigned short weight, weapontype weaptype, unsigned char capacity) {
	return (Item) {firearm, name, bulk, weight, .properties.fir = {weaptype, capacity}};
}

/*** Returns a list with all the game objects ***/
Item *getgameobjects() {
	Item *objects = malloc(50 * sizeof(Item));
	objects[0] = newfirearm("a gun", 500, 5000, handgun, 10);
	objects[1] = newcar("a car", 3500, 4000, 200);
 	objects[2] = newbullet("a bullet", 200, 3000, 300, 5000);
	return objects;
}

/*** Bullet getters ***/
unsigned short getvelocity(short bullet_id) {
	assert(objects[bullet_id].type == bullet); 
	return objects[bullet_id].properties.bul.velocity;
}

unsigned short getkineticenergy(short bullet_id) {
	assert(objects[bullet_id].type == bullet); 
	return objects[bullet_id].properties.bul.kineticenergy;
}

/*** Car getters ***/
unsigned short getmaxspeed(short car_id) {
	assert(objects[car_id].type == car);
	return objects[car_id].properties.car.maxspeed;
}

/*** Firearm getters ***/
weapontype getweaptype(short firearm_id) {
	assert(objects[firearm_id].type == firearm);
	return objects[firearm_id].properties.fir.weaptype;
}

unsigned char getcapacity(short firearm_id) {
	assert(objects[firearm_id].type == firearm);
	return objects[firearm_id].properties.fir.capacity;
}


/*
int main() {
	objects = getgameobjects();
	if(getweaptype(0) == handgun) {
		printf("handgun\n");
	}
	printf("%d \n", getcapacity(0));
	printf("%d \n", getmaxspeed(1));
	printf("%d \n", getkineticenergy(2));
	printf("%d \n", getvelocity(2));
	free((Item*) objects);
	return 0;
}
*/
