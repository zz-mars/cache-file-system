#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "stack.h"

typedef struct _tree_node{
	char c;
	struct _tree_node * p;
	struct _tree_node * l;
	struct _tree_node * r;
}tnode;

#define c(tn)	(tn)->c
#define p(tn)	(tn)->p
#define l(tn)	(tn)->l
#define r(tn)	(tn)->r

#define TNODE_LC	0
#define TNODE_RC	1

static tnode * root = NULL;

static tnode *newTnode(char c)
{
	tnode *nt = (tnode*)malloc(sizeof(tnode));
	if(!nt) {
		perror("malloc new tnode");
		return NULL;
	}

	c(nt) = c;
	p(nt) = l(nt) = r(nt) = NULL;

	return nt;
}

static void addNewTnode(tnode *newNode,tnode *parent,char lr)
{
	assert(lr == TNODE_LC || lr == TNODE_RC);

	if(!parent) {
		if (root) {
			fprintf(stderr,"root has been set,please specify parent node!\n");
			return;
		}
		root = parent;
	}

	if(lr == TNODE_LC) {
		l(parent) = newNode;
	} else if(lr == TNODE_RC) {
		r(parent) = newNode;
	}

	p(newNode) = parent;
}

static void inOrderTraverse(tnode * r)
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
	return;
}

static void inOrderTraverseNoRecur(tnode *r)
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

static void preOrderTraverse(tnode * r)
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

static void preOrderTraverseNoRecur(tnode * r)
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

static void postOrderTraverse(tnode * r)
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

static void postOrderTraverseNoRecur(tnode * r)
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
			/* backtrace */
backtrace:
			printf("%c ",c(t));

			if((s = postOrderStack->top(postOrderStack)) == NULL){
				break;
			}

			if(t == l(s) || t == r(s)) {
				/* if t is child of s,s should be pop out */
				t = s;
				postOrderStack->pop(postOrderStack);
				goto backtrace;
			}
		}
	}
	printf("\n");
	delStack(postOrderStack);
}
static void postOrderTraverseNoRecur_(tnode * r)
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
			/* backtrace */

			do {
				printf("%c ",c(t));

				tnode *s;
				if((s = postOrderStack->top(postOrderStack)) == NULL){
					goto ret;
				}

				if(t == l(s) || t == r(s)) {
					/* if t is child of s,s should be pop out */
					/* backtrace */
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

#define OPERATORS_N	7
static char operators[OPERATORS_N] = {'+','-','*','/','(',')','#'};

#define IS_OPERATOR(c) ({	\
		char _c = c;	\
		int i = 0;	\
		for(i=0;i<OPERATORS_N;i++) {	\
			if (operators[i] == _c) {	\
				break;	\
			}	\
		}	\
		(i < OPERATORS_N ? i : -1);	\
})

#define IS_EXPR(c)	({	\
		char _c = c;	\
		((_c <= 'z' && _c >= 'a') ||	\
		 (_c <='Z' && _c >= 'A')	||	\
		 (_c <= '9' && _c >= '0') );	\
})

static int privilege_table[OPERATORS_N][OPERATORS_N] = {	\
	1,1,-1,-1,-1,1,1,	\
	1,1,-1,-1,-1,1,1,	\
	1,1,1,1,-1,1,1,	\
	1,1,1,1,-1,1,1,	\
	-1,-1,-1,-1,-1,0,2,	\
	1,1,1,1,2,1,1,	\
	-1,-1,-1,-1,-1,2,0	\
};

int expr_to_tree(char * expr)
{
	stack *opStack = newStack(STK_SZ);
	assert(opStack != NULL);
	stack *expStack = newStack(STK_SZ);
	assert(expStack != NULL);

	int i = 0;
	int sl = strlen(expr);
	while(i < sl) {
		int j = IS_OPERATOR(expr[i]);
		if(j >= 0) {

			/* operator */
			tnode *tn = newTnode(expr[i]);
			assert(tn != NULL);

			if(opStack->empty(opStack)) {
				/* push to operator stack when it is null */
				opStack->push(opStack,tn);
				i++;
				continue;
			}

			tnode *top = opStack->top(opStack);
			assert(top != NULL);

			int k = IS_OPERATOR(c(top));
			assert(k>=0 && k <= OPERATORS_N);

			switch(privilege_table[k][j]) {
				case 1:
					{
						/* new operator is less privileged than the former one */
						tnode *tr = expStack->pop(expStack);
						assert(tr != NULL);

						tnode *tl = expStack->pop(expStack);
						assert(tl != NULL);

						tnode *tp = opStack->pop(opStack);
						assert(tp != NULL);

						/* make a tree */
						addNewTnode(tl,tp,TNODE_LC);
						addNewTnode(tr,tp,TNODE_RC);
						expStack->push(expStack,tp);
					}
					break;
				case -1:
					opStack->push(opStack,tn); 
					i++;
					break;
				case 0:
					{
						tnode *t = opStack->pop(opStack);
						free(t);
						free(tn);
						i++;
					}
					break;
				default:
					fprintf(stderr,"invalid expression\n");
					return -1;
			}
		}else if(IS_EXPR(expr[i])) {
			tnode *tn = newTnode(expr[i]);
			assert(tn != NULL);
			expStack->push(expStack,tn);
			i++;
		}else {
			fprintf(stderr,"invalid expression\n");
			return -1;
		}
	}

	root = expStack->pop(expStack);
	assert(root != NULL);

	/* all stack should be empty */
	assert(expStack->empty(expStack) && opStack->empty(opStack));

	delStack(opStack);
	delStack(expStack);

	return 0;
}

int main()
{
	char *s = "#a+b*(c-d)-e/f#";

	expr_to_tree(s);

	printf("inOrderTraverse :\n");
	inOrderTraverse(root);
	printf("\n");

	printf("inOrderTraverseNoRecur :\n");
	inOrderTraverseNoRecur(root);


	printf("preOrderTraverse :\n");
	preOrderTraverse(root);
	printf("\n");

	printf("preOrderTraverseNoRecur :\n");
	preOrderTraverseNoRecur(root);


	printf("postOrderTraverse :\n");
	postOrderTraverse(root);
	printf("\n");

	printf("postOrderTraverseNoRecur :\n");
	postOrderTraverseNoRecur(root);

	printf("postOrderTraverseNoRecur_ :\n");
	postOrderTraverseNoRecur_(root);

	return 0;
}

