#define THREADING_TREE

#include "binary_tree.h"

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
void inOrderThreading(tnode *r,tnode *head)
{
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

		/* next of last points to head */
		r(pre) = head;
		rf(pre) = LR_FLAG_THREADING;
		r(head) = pre;
	}
}

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

