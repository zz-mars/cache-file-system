#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "stack.h"

static int push(struct stack *this,void * c);
static void * pop(struct stack *this);
static void * top(struct stack *this);
static int empty(struct stack *this);

stack * newStack(int msize)
{
	stack *s = (stack *)malloc(sizeof(stack));

	if (!s) {
		perror("malloc for stack");
		return NULL;
	}

	s->msize = msize;

	if((s->m = (void**)malloc(sizeof(void*) * s->msize)) == NULL) {
		perror("malloc for stack->m");
		free(s);
		return NULL;
	}

	s->base = 0;
	s->sp = 0;
	s->push = push;
	s->pop = pop;
	s->top = top;
	s->empty = empty;

	return s;
}

void delStack(stack * s)
{
	if(s->m) {
		free(s->m);
	}
	free(s);
}

static int push(struct stack *this,void * c)
{
	if(this->sp >= this->msize) {
		fprintf(stderr,"stack full,cannot push\n");
		return 1;
	}

	this->m[this->sp++] = c;

	return 0;
}

static void * pop(struct stack *this)
{
	return this->sp <= 0 ? NULL : this->m[--this->sp];
}

static void * top(struct stack *this)
{
	return this->sp <= 0 ? NULL : this->m[this->sp-1];
}

static int empty(struct stack * s)
{
	return s->sp == 0 ? 1:0;
}

