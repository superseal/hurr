#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct listnode {
	unsigned char id; /* Object ID */
	unsigned char state; /* Object specific state */
	struct listnode *next;
} Node;

typedef struct linkedlist {
	struct listnode *header; /* Sentinel node */
	unsigned int length;
} Inventory;

/* Create new node (seriously) */
Node *node_create(char id, char state, Node *next);

/* Create linked list with a sentinel node */
Inventory *list_create();

/* Insert new item with values (id, state) to inventory */
void list_insert(Inventory *list, unsigned int position, char id, char state); 

/* Remove item i from inventory */
void list_remove(Inventory *list, unsigned int position);

/* Apply func() to every node in the list */
int list_foreach(Inventory *list, int(*func)(Node*));

/* Returns position of element with matching ID and state */
unsigned int list_find(Inventory *list, char id, char state);

/* Swap ID and state of nodes at pos_0 and pos_1 */
void list_swap(Inventory *list, unsigned int pos_0, unsigned int pos_1);
