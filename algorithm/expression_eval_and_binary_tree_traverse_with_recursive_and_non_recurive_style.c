#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "stack.h"

#define THREADING_TREE

typedef struct _tree_node{
	char c;
	struct _tree_node * p;
	struct _tree_node * l;
	struct _tree_node * r;

#ifdef THREADING_TREE
	char lf;
	char rf;
#endif

}tnode;

#ifdef THREADING_TREE
enum {
	LR_FLAG_CHILD = 1,
	LR_FLAG_THREADING
};
static tnode head;
#endif

#define c(tn)	(tn)->c
#define p(tn)	(tn)->p
#define l(tn)	(tn)->l
#define r(tn)	(tn)->r

#ifdef THREADING_TREE
#define lf(t)	(t)->lf
#define rf(t)	(t)->rf
#endif

enum {
	TNODE_LC = 1,
	TNODE_RC
};

static tnode * root = NULL;

static tnode *newTnode(char c)
{
	tnode *nt = (tnode*)malloc(sizeof(tnode));
	memset(nt,0,sizeof(tnode));
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
#ifdef THREADING_TREE
		lf(parent) = LR_FLAG_CHILD;
#endif
	} else if(lr == TNODE_RC) {
		r(parent) = newNode;
#ifdef THREADING_TREE
		rf(parent) = LR_FLAG_CHILD;
#endif
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

#ifdef THREADING_TREE

/* traverse alone inOrder thread */
static void inOrderTraverseThread(tnode * h)
{
	tnode *t = l(h);
	while(t != h) {
		/* find the start point of every loop */
		while(lf(t) == LR_FLAG_CHILD){
			t = l(t);
		}

		printf("%c ",c(t));

		/* traverse along thread */
		while(rf(t) == LR_FLAG_THREADING && r(t) != h) {
			t = r(t);
			printf("%c ",c(t));
		}
		/* when rf(t) == LR_FLAG_CHILD,
		 * find next element in the right branch,
		 * which will be the most left one of the right branch */
		t = r(t);
	}
	printf("\n");
	return;
}

/* threading helper function */
static void inThreading(tnode *t,tnode **pre)
{
	if(t) {
		inThreading(l(t),pre);
		if(!l(t)) {
			l(t) = *pre;
			lf(t) = LR_FLAG_THREADING;
		}
		if(!r(*pre)) {
			r(*pre) = t;
			rf(*pre) = LR_FLAG_THREADING;
		}
		*pre = t;
		inThreading(r(t),pre);
	}
}

/* make the thread,return head of new inOrderThread */
static tnode * inOrderThreading(tnode *r)
{
	tnode *head = newTnode('h');
	assert(head != NULL);

	lf(head) = LR_FLAG_CHILD;
	rf(head) = LR_FLAG_THREADING;

	r(head) = head;

	if(!r) {
		l(head) = head;
	}else{
		l(head) = r;
		tnode * pre = head;
		/* make thread */
		inThreading(r,&pre);
		r(pre) = head;
		rf(pre) = LR_FLAG_THREADING;
		r(head) = pre;
	}

	return head;
}

#endif

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

#ifdef THREADING_TREE

/* postOrderTraverseThread
 * @r : root of the tree,end of the traverse
 */
static void postOrderTraverseThread(tnode *r,tnode *head)
{
	tnode *t = l(head);


	while(t != head) {
		/* find the start point of this loop */
		while ( (lf(t) == LR_FLAG_CHILD ) || (rf(t) == LR_FLAG_CHILD )) {

			if(lf(t) == LR_FLAG_CHILD ) {
				t = l(t);
				continue;
			}

			if(rf(t) == LR_FLAG_CHILD ) {
				t = r(t);
			}
		}

		printf("%c ",c(t));

		while(rf(t) == LR_FLAG_THREADING && r(t) != head) {
			t = r(t);
			printf("%c ",c(t));
		}

		if(r(t) == head) {
			break;
		}

		/* find next element when thread is broken */
		if( (lf(p(t)) == LR_FLAG_CHILD && 
					t == l(p(t)) && rf(p(t)) != LR_FLAG_CHILD) || 
				(rf(p(t)) == LR_FLAG_CHILD && t == r(p(t))) ) {
			/* 1) when t is left child of its parent who has no right child
			 * 2) when t is right child of its parent
			 * next element is its parent */
			t = p(t);
		}else if( lf(p(t)) == LR_FLAG_CHILD && t == l(p(t)) && //t is left child 
				rf(p(t)) == LR_FLAG_CHILD ){	//right child exist
			/* both children exist,next element is the first element visited in the right branch */
			t = r(p(t));
		}

	}

	printf("\n");
	return;
}

#endif

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

#ifdef THREADING_TREE
	tnode *head = inOrderThreading(root);
	inOrderTraverseThread(head);
#endif

//	printf("preOrderTraverse :\n");
//	preOrderTraverse(root);
//	printf("\n");
//
//	printf("preOrderTraverseNoRecur :\n");
//	preOrderTraverseNoRecur(root);
//
//
//	printf("postOrderTraverse :\n");
//	postOrderTraverse(root);
//	printf("\n");
//
//	printf("postOrderTraverseNoRecur :\n");
//	postOrderTraverseNoRecur(root);
//
//	printf("postOrderTraverseNoRecur_ :\n");
//	postOrderTraverseNoRecur_(root);


	return 0;
}

