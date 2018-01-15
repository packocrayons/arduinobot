#include <stdio.h>

#include "stack.c"




void function1(int number){
	printf("This is function 1 printing argument : %i\n", number);
}


void function2(int number){
	printf("This is function 2 printing argument : %i\n", number);
}


void function3(int number){
	printf("This is function 3 printing argument : %i\n", number);
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
		popAndExecute(&head);
	}
}