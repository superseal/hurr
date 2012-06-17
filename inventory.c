#include "inventory.h"

/*** Create new node (seriously) ***/
Node *node_create(char id, char state, Node *next) {
	Node *node = malloc(sizeof(Node));
	if (node == NULL) {return NULL;}
	node->id = id;
	node->state = state;
	node->next = next;
	return node;
}

/*** Create linked list with a sentinel node ***/
Inventory *list_create() {
	Node *header = node_create(-1, -1, NULL);
	Inventory *itemlist = malloc(sizeof(Node));
	itemlist->header = header;
	itemlist->length = 0;
	return itemlist;
}

/*** Insert new item with values (id, state) to inventory ***/
void list_insert(Inventory *list, unsigned int position, char id, char state) {
	if (position > list->length) {return;}
	Node *currentnode = list->header;
	unsigned char index; 
	for (index = 0; index < position; index++) {
		currentnode = currentnode->next;
	}
	Node *newnode = node_create(id, state, currentnode->next);
	currentnode->next = newnode;
	(list->length)++;
}

/*** Remove item i from inventory ***/
void list_remove(Inventory *list, unsigned int position) {
	if (position >= list->length) {return;}
	Node *currentnode = list->header;
	unsigned char index; 
	for (index = 0; index < position; index++) {
		currentnode = currentnode->next;
	}
	Node *deletednode = currentnode->next;
	currentnode->next = currentnode->next->next;
	free(deletednode);
	(list->length)--;
}

/*** Apply func() to every node in the list ***/
int list_foreach(Inventory *list, int(*func)(Node*)) {
	Node *currentnode = (list->header)->next;
	unsigned char index;
	/* Don't count header node, start at 1 */
	for (index = 1; index <= list->length; index++) {
		if (func(currentnode) != 0) {return -1;}
		currentnode = currentnode->next;
	}
	return 0;
}

/*** Returns position of element with matching ID and state ***/
unsigned int list_find(Inventory *list, char id, char state) {
	Node *currentnode = list->header;
	unsigned int position = -1, index;
	for (index = 0; index < list->length; index++) {
		if (currentnode->id == id && currentnode->state == state) {
			return position;
		}
		currentnode = currentnode->next; 
		position++;
	}
	return -1;
}

/*** Swap ID and state of nodes at pos_0 and pos_1 ***/
void list_swap(Inventory *list, unsigned int pos_0, unsigned int pos_1) {
	Node *currentnode = list->header;
	/* Sort variables to avoid overhead when iterating through linked list */
	if (pos_0 > pos_1) {
		unsigned int temp = pos_0;
		pos_0 = pos_1;
		pos_1 = temp;
	}
	/* Once they're sorted, check indexes */
	if (pos_1 >= list->length || pos_0 == pos_1) {return;}
	unsigned char index;
	/* Get pos_0 address and save */
	for (index = 0; index <= pos_0; index++) {
		currentnode = currentnode->next;
	}
	Node *node_0 = currentnode;
	/* Get pos_1 address and save */
	for (index = 0; index < (pos_1 - pos_0); index++) {
		currentnode = currentnode->next;
	}
	Node *node_1 = currentnode;
	/* Swap id and state */
	char tid = node_0->id, tstate = node_0->state;
	node_0->id = node_1->id; node_0->state = node_1->state;
	node_1->id = tid; node_1->state = tstate;
}
