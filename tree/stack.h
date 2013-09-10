#ifndef _STACK_H
#define _STACK_H

#define STK_SZ	1024

typedef struct stack {
	void **m;	//memory zone
	int msize;	//mem size
	int sp;		//index of top element plus one
	int base;
	int (*push)(struct stack *this,void * c);
	void * (*pop)(struct stack *this);
	void * (*top)(struct stack *this);
	int (*empty)(struct stack *this);
}stack;

extern stack * newStack(int msize);
extern void delStack(stack * s);

#endif
