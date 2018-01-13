//This is just an example linked list where the stackElements have a function pointer and argument. This will be later used to make the decision stack for the bot, with proper functions

#include <stdio.h>
#include <stdlib.h>

typedef struct stackElement {
	struct stackElement* next; //next may be the wrong word here, since head (which also may be the wrong word) is actually the end of the list, and head->next is null at the leaf node.
	int arg;
	void (*function)(int);
} stackElement;


void function1(int number){
	printf("This is function 1 printing argument : %i\n", number);
}


void function2(int number){
	printf("This is function 2 printing argument : %i\n", number);
}


void function3(int number){
	printf("This is function 3 printing argument : %i\n", number);
}


void addToHead(stackElement** head, stackElement* toAdd){
	toAdd->next = *head;
	*head = toAdd;
}

void removeAndFreeHead(stackElement** head){ //replaces head with head-> next
	stackElement* prevHead = *head;
	*head = (*head)->next;
	free(prevHead);
}

int main(int argc, char* argv[]){
	stackElement* head = (stackElement*) malloc(sizeof(stackElement));
	head->next=NULL;


	stackElement* newElement;

	newElement = (stackElement*) malloc(sizeof(stackElement));
	newElement->arg = 1;
	newElement->function = &function1; //New element->function now contains a function pointer to function 1. Now, we can call newElement->function(newElement->arg) and it's the same as calling function1(1);
	addToHead(&head, newElement);


	newElement = (stackElement*) malloc(sizeof(stackElement)); //make it a new pointer so we don't clobber the old one that's sitting in the linked list
	newElement->arg = 2;
	newElement->function = &function2; 
	addToHead(&head, newElement);


	newElement = (stackElement*) malloc(sizeof(stackElement));
	newElement->arg = 3;
	newElement->function = &function3; 
	addToHead(&head, newElement);

	
	while(head->next != NULL){
		head->function(head->arg);
		removeAndFreeHead(&head);
	}
}