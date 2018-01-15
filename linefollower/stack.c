#include <stdlib.h>


/*
This is the struct definition and helper functions to create a call stack to undo actions done by the robot when it hits a dead end.
The robot should execute the element off the top of the stack, and then call removeAndFreeHead.
Every time it executes something that was not popped of the stack (ie it's not in recall mode), it should put the OPPOSITE instruction on the stack.
*/


typedef struct stackElement {
	struct stackElement* next; //next may be the wrong word here, since head (which also may be the wrong word) is actually the end of the list, and head->next is null at the leaf node.
	int arg;
	void (*function)(int);
} stackElement;


void addToHead(stackElement** head, stackElement* toAdd){
	toAdd->next = *head;
	*head = toAdd;
}

void removeAndFreeHead(stackElement** head){ //replaces head with head-> next
	stackElement* prevHead = *head;
	*head = (*head)->next;
	free(prevHead);
}

void popAndExecute(stackElement** head){
	stackElement* executor = *head;
	executor->function(executor->arg);
	removeAndFreeHead(head);
}