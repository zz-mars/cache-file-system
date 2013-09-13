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
}

void postOrderTraverseNoRecur(tnode * r)
{
	stack * postOrderStack = newStack(STK_SZ);
	tnode *t = r;
	postOrderStack->push(postOrderStack,t);

	while(!postOrderStack->empty(postOrderStack)) {
		t = postOrderStack->top(postOrderStack);

		int cont = 0;
		/* push child on stack
		 * right child first */
		if(r(t)) {
			postOrderStack->push(postOrderStack,r(t));
			cont = 1;
		}

		if(l(t)) {
			postOrderStack->push(postOrderStack,l(t));
			cont = 1;
		}

		if(cont) {
			continue;
		}

		while(!postOrderStack->empty(postOrderStack)) {
			t = postOrderStack->pop(postOrderStack);
			printf("%c ",c(t));
			tnode *s = postOrderStack->top(postOrderStack);

			if(!s) {
				goto post_trav_over;
			}

			if(t == l(s) || t == r(s)) {
				/* if t is child of s,s should be pop out */
				continue;
			}
			/* t and s are brothers, 
			 * move to right side */
			break;
		}
	}
post_trav_over:
	printf("\n");
	delStack(postOrderStack);
}

