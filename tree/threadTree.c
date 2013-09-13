#include "binary_tree.h"

static tnode *in_pre;

static void inThreading(tnode *t)
{
	if(t) {
		inThreading(l(t));
		if(!l(t)) {
			l(t) = in_pre;
			lf(t) = LR_FLAG_THREADING;
		}
		if(!r(in_pre)) {
			r(in_pre) = t;
			rf(in_pre) = LR_FLAG_THREADING;
		}
		in_pre = t;
		inThreading(r(t));
	}
}

void inOrderThreading(tnode *r,tnode *head)
{
	lf(head) = LR_FLAG_CHILD;
	rf(head) = LR_FLAG_THREADING;

	r(head) = head;

	if(!r) {
		l(head) = head;
	}else{
		l(head) = r;

		in_pre = head;
		/* make thread */
		inThreading(r);

		/* in_pre points to the last element,
		 * in_pre->right_child should point to head */
		r(in_pre) = head;
		rf(in_pre) = LR_FLAG_THREADING;
		r(head) = in_pre;
	}
}

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

tnode *post_pre;

void postThreading(tnode *t)
{
	if(!t) {
		return;
	}

	postThreading(l(t));
	postThreading(r(t));

	if(!l(t)) {
		l(t) = post_pre;
		lf(t) = LR_FLAG_THREADING;
	}

	if(!r(post_pre)) {
		r(post_pre) = t;
		rf(post_pre) = LR_FLAG_THREADING;
	}

	post_pre = t;
}

void postOrderThreading(tnode *r,tnode *head)
{
	l(head) = r;
	lf(head) = LR_FLAG_CHILD;
	r(head) = head;
	rf(head) = LR_FLAG_THREADING;
	post_pre = head;

	postThreading(r);

	/* post_pre now points to root */
	assert(post_pre == r);
	/* set parent of root to head */
	p(post_pre) = head;

	if(!r(post_pre)) {
		r(post_pre) = head;
		rf(post_pre) = LR_FLAG_THREADING;
	}
}

/* return the tnode which will be traversed first */
static tnode *find_start_point(tnode *r)
{
	tnode *x = r;
	while(1) {
		/* find the most left node */
		while(lf(x) == LR_FLAG_CHILD && l(x)) {x=l(x);}
		/* if this node has right child */
		if(rf(x) == LR_FLAG_CHILD && r(x)) {
			x = r(x);
			continue;
		}
		break;
	}
	return x;
}

static tnode *find_start_point_recur(tnode *r)
{
	tnode *x = r;
	/* find the most left node */
	while(lf(x) == LR_FLAG_CHILD && l(x)) {x=l(x);}
	/* if this node has right child */
	if(rf(x) == LR_FLAG_CHILD && r(x)) {
		return find_start_point_recur(r(x));
	}
	return x;
}

void postOrderTraverseThread(tnode *head)
{
	tnode *x = l(head);
	while(x != head) {
		x = find_start_point(x);
		printf("%c ",c(x));
		while(rf(x) == LR_FLAG_THREADING) {
			x = r(x);
			if(x == head) {
				printf("\n");
				return;
			}
			printf("%c ",c(x));
		}
		/* when thread breaks */
		while(x != head) {
			if(rf(p(x)) == LR_FLAG_CHILD && x == r(p(x))) {
				/* backtrack when x is right child */
				x = p(x);
				printf("%c ",c(x));
			} else if(lf(p(x)) == LR_FLAG_CHILD && x == l(p(x))) {
				/* x is left child */
				if(r(p(x))) {
					/* move to right side */
					x = r(p(x));
					break;
				}else {
					/* backtrack when its parent has no right child */
					x = p(x);
					printf("%c ",c(x));
				}
			}
		}
	}
	printf("\n");
}

static tnode *pre_pre;

static preThreading(tnode *r)
{
	if(!r) {
		return;
	}

	if(!l(r)) {
		l(r) = pre_pre;
		lf(r) = LR_FLAG_THREADING;
	}

	if(!r(pre_pre)) {
		r(pre_pre) = r;
		rf(pre_pre) = LR_FLAG_THREADING;
	}

	pre_pre = r;

	if(lf(r) == LR_FLAG_CHILD) {
		preThreading(l(r));
	}
	preThreading(r(r));
}

void preOrderThreading(tnode *r,tnode *head)
{
	l(head) = r;
	lf(head) = LR_FLAG_CHILD;
	r(head) = head;
	rf(head) = LR_FLAG_THREADING;

	pre_pre = head;

	preThreading(r);

	r(pre_pre) = head;
	rf(pre_pre) = LR_FLAG_THREADING;
}

void preOrderTraverseThread(tnode *head)
{
	tnode *x = l(head);

	while(x != head) {
		while(x && x != head) {
			printf("%c ",c(x));
			if(rf(x) == LR_FLAG_THREADING && r(x)) {
				x = r(x);
				continue;
			}
			break;
		}

		if(!x || x == head) {
			break;
		}

		if(lf(x) == LR_FLAG_CHILD && l(x)) {
			x = l(x);
			continue;
		}

		if(rf(x) == LR_FLAG_CHILD && r(x)){
			x = r(x);
		}
	}
	printf("\n");
}

