#include "stack.c"
#include <stdio.h>

int STACK_SIZE = (20 * sizeof(stackElement));


void printStack(Stack* st){
	printf("The stack starts at: %p and is %d bytes long\n", st->mem, STACK_SIZE);
	printf("The top is at %p\n",st->head);
	for(int i = 0; i < STACK_SIZE; i += sizeof(stackElement)){
		void* ptr = (((stackElement*) st->mem) + i); //the cast to stackElement* here is just to get the size to work with the +i, you can't do arithmetic with void pointers
		printf("%p ", ptr);
		if (ptr == st->mem){
			printf("root ->\t");
		}
		else if (((stackElement*) ptr) == st->head){
			printf("top  ->\t");
		}
		else {
			printf("\t\t");
		}
		printf("%p (%i)\n", ((stackElement*) ptr)->function, ((stackElement*)ptr)->arg);
	}
}

void function1(int x){
	printf("Hi I'm function 1 and you sent me %i\n", x);
}

void function2(int x){
	printf("Hi I'm function 2 and you sent me %i\n", x);
}

void function3(int x){
	printf("Hi I'm function 3 and you sent me %i\n", x);
}

void function4(int x){
	printf("Hi I'm function 4 and you sent me %i\n", x);
}



int main(int argc, char* argv[]){
	Stack* stack = init(STACK_SIZE); //initialize a stack of 20 bytes

	printf("a stackElement is %i bytes long\n", (int) sizeof(stackElement));

	// printStack(stack);

	stackElement temp;

	temp.function=function1;
	temp.arg=1;
	push(stack, &temp);

	temp.function=function2;
	temp.arg=2;
	push(stack, &temp);

	temp.function=function3;
	temp.arg=3;
	push(stack, &temp);

	temp.function=function4;
	temp.arg=4;
	push(stack, &temp);

	temp.function=function1;
	temp.arg=1;
	push(stack, &temp);


	stackElement* popped = NULL;
	for (int i = 0; i < 5; ++i)
	{
		printStack(stack);
		popped = pop(stack);
		popped->function(popped->arg);
	}
}
