#include <stdlib.h>


/*
This is the struct definition and helper functions to create a call stack to undo actions done by the robot when it hits a dead end.
The robot should execute the element off the top of the stack, and then call removeAndFreeHead.
Every time it executes something that was not popped of the stack (ie it's not in recall mode), it should put the OPPOSITE instruction on the stack.
*/


// typedef struct stackElement{
// 	char c;
// }stackElement;

typedef struct stackElement {
	int arg;
	void (*function)(int);
} stackElement;

typedef struct Stack{
	void* mem;
	stackElement* head;
} Stack;

//Changed 'setup()' to 'init()' to avoid duplicate prototypes
Stack* init(int memSize){ //returns a void pointer to the entire stack
	Stack* stack = (Stack*) malloc(sizeof(stack));
	stack->mem = malloc(memSize);
	stack->head = ((stackElement*) stack->mem) - sizeof(stackElement);
}

void push(Stack* st, stackElement* toAdd){
	st->head = (stackElement*) (st->head + sizeof(stackElement)); //shift the pointer to the next location
	*st->head = *toAdd; //the VALUE of head (what's actually in memory) is equal to the VALUE of toAdd
}

/*IMPORTANT NOTE - this function basically returns a pointer to the top of the stack. This means that the following:
stackElement* var = pop(stack);
stackElement* newStackElement = [declaration];
push(stack, newStackElement);
execute(var)
Will fail, and newStackElement will atually be executed. This is because the top of the stack is freed for replacement.
The reason it is done this way is it is assumed that pop will be used immediately and then be thrown away.
An alternative would be to have a reserved space (an eden space, per se) for the most recently popped, and it will only be replaced on a new pop.
This is a viable option and is up for discussion. */
stackElement* pop(Stack* st){
	stackElement* temp = (stackElement*) st->head;
	st->head = (stackElement*) (st->head - sizeof(stackElement));
	return temp;
}
