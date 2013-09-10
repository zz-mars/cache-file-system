#include "binary_tree.h"
#include "stack.h"

void addNewTnode(tnode *newNode,tnode *parent,char lr)
{
	assert(lr == TNODE_LC || lr == TNODE_RC);

//	if(!parent) {
//		if (root) {
//			fprintf(stderr,"root has been set,please specify parent node!\n");
//			return;
//		}
//		root = parent;
//	}
	
	if(!parent || !newNode) {
		return;
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
	return;
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

#ifdef THREADING_TREE

/* traverse alone inOrder thread */
void inOrderTraverseThread(tnode * h)
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
tnode * inOrderThreading(tnode *r)
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

#ifdef THREADING_TREE

tnode *pre;

void postInThreading(tnode *t)
{
	if(!t) {
		return;
	}

	postInThreading(l(t));
	postInThreading(r(t));

	if(!l(t)) {
		l(t) = pre;
		lf(t) = LR_FLAG_THREADING;
	}

	if(!r(pre)) {
		r(pre) = t;
		rf(pre) = LR_FLAG_THREADING;
	}

	pre = t;
}

tnode * postOrderThreading(tnode *r)
{
	tnode *head = newTnode('n');
	l(head) = r;
	r(head) = head;
	pre = head;

	postInThreading(r);

	r(head) = pre;
	rf(head) = LR_FLAG_THREADING;
	if(!r(pre)) {
		r(pre) = head;
		rf(pre) = LR_FLAG_THREADING;
	}
	return head;
}

/* postOrderTraverseThread
 * @r : root of the tree,end of the traverse
 */
void postOrderTraverseThread(tnode *r,tnode *head)
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

