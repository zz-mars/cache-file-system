#include "binary_tree.h"
#include "stack.h"

void inOrderTraverse(tnode * r)
{
	if (!r) {
		return;
	}

	if(l(r)) {
		inOrderTraverse(l(r));
	}
	printf("%c ",c(r));
	if(r(r)) {
		inOrderTraverse(r(r));
	}
}

void inOrderTraverseNoRecur(tnode *r)
{
	stack * inOrderStack = newStack(STK_SZ);
	tnode *t = r;
	while (t || !inOrderStack->empty(inOrderStack)) {
		if(t) {
			inOrderStack->push(inOrderStack,t);
			t = l(t);
		}else {
			t = inOrderStack->pop(inOrderStack);
			printf("%c ",c(t));
			t = r(t);
		}
	}
	printf("\n");
	delStack(inOrderStack);
}

void preOrderTraverse(tnode * r)
{
	if (!r) {
		return;
	}
	printf("%c ",c(r));
	if(l(r)) {
		preOrderTraverse(l(r));
	}
	if(r(r)) {
		preOrderTraverse(r(r));
	}
	return;
}

void preOrderTraverseNoRecur(tnode * r)
{
	tnode *t = r;
	stack * preOrderStack = newStack(STK_SZ);

	while (t || !preOrderStack->empty(preOrderStack)) {
		if(t) {
			printf("%c ",c(t));

			if(r(t)) {
				/* save right child */
				preOrderStack->push(preOrderStack,r(t));
			}
			t = l(t);
		}else {
			t = preOrderStack->pop(preOrderStack);
		}
	}
	printf("\n");
	delStack(preOrderStack);
}

void postOrderTraverse(tnode * r)
{
	if (!r) {
		return;
	}
	if(l(r)) {
		postOrderTraverse(l(r));
	}
	if(r(r)) {
		postOrderTraverse(r(r));
	}
	printf("%c ",c(r));
	return;
}

void postOrderTraverseNoRecur(tnode * r)
{
	stack * postOrderStack = newStack(STK_SZ);
	tnode *t = r;
	postOrderStack->push(postOrderStack,t);
	int stack_dep = 0;

	while(!postOrderStack->empty(postOrderStack)) {
		t = postOrderStack->top(postOrderStack);

		if(t) {
			postOrderStack->push(postOrderStack,r(t));
			postOrderStack->push(postOrderStack,l(t));
		}else {
			/* eat the null */
			while ((t = postOrderStack->pop(postOrderStack)) == NULL);
			tnode *s;
			/* backtrack */
backtrack:
			printf("%c ",c(t));

			if((s = postOrderStack->top(postOrderStack)) == NULL){
				break;
			}

			if(t == l(s) || t == r(s)) {
				/* if t is child of s,s should be pop out */
				t = s;
				postOrderStack->pop(postOrderStack);
				goto backtrack;
			}
		}
	}
	printf("\n");
	delStack(postOrderStack);
}

void postOrderTraverseNoRecur_(tnode * r)
{
	stack * postOrderStack = newStack(STK_SZ);
	tnode *t = r;
	postOrderStack->push(postOrderStack,t);
	int stack_dep = 0;

	while(!postOrderStack->empty(postOrderStack)) {
		t = postOrderStack->top(postOrderStack);

		if(t) {
			postOrderStack->push(postOrderStack,r(t));
			postOrderStack->push(postOrderStack,l(t));
		}else {
			/* eat the null */
			while ((t = postOrderStack->pop(postOrderStack)) == NULL);

			/* backtrack */
			do {
				printf("%c ",c(t));

				tnode *s;
				if((s = postOrderStack->top(postOrderStack)) == NULL){
					goto ret;
				}

				if(t == l(s) || t == r(s)) {
					/* if t is child of s,s should be pop out */
					/* backtrack */
					t = postOrderStack->pop(postOrderStack);
					if(postOrderStack->empty(postOrderStack)) {
						printf("%c ",c(t));
					}
				} else {
					/* t and s are brothers */
					break;
				}
			}while(!postOrderStack->empty(postOrderStack));
		}
	}
ret:
	printf("\n");
	delStack(postOrderStack);
}
