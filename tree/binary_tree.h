#ifndef _BIN_TREE_H
#define _BIN_TREE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

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

static inline tnode *newTnode(char c)
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

static inline void delTnode(tnode *t)
{
	if(!t) {
		fprintf(stderr,"invalid arg!\n");
		return;
	}
	free(t);
}

void addNewTnode(tnode *newNode,tnode *p,char lr);

void inOrderTraverse(tnode *r);
void inOrderTraverseNoRecur(tnode *r);
tnode * inOrderThreading(tnode *r);
void inOrderTraverseThread(tnode *r);

void preOrderTraverse(tnode *r);
void preOrderTraverseNoRecur(tnode *r);

void postOrderTraverse(tnode *r);
void postOrderTraverseNoRecur(tnode *r);
void postOrderTraverseNoRecur_(tnode *r);
void postOrderTraverseThread(tnode *r,tnode *head);

#endif
