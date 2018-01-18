# ArduinoBot

This is a repository for the code running on an arduino robot destined for solving a maze using a stack and retreat technique.

## Solving technique

The maze is solved by tracking what has been executed on a stack, and when a dead-end is reached, retreating to the previous decision and taking the opposite.

This is done by storing a function pointer (and associated argument, all instruction functions take a single int) on a stack, where when popped and executed, this function will undo the action that was just done.

## Stack

The stack is present in `stack.c`, and is a raw stack of structs (stackElements). This is currently on a separate branch and the master branch uses a linked list.