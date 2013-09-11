#ifndef _BIN_TREE_H
#define _BIN_TREE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct tnode{
	char c;
	struct tnode * p;
	struct tnode * l;
	struct tnode * r;
	void *data;

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

#define lf(t)	(t)->lf
#define rf(t)	(t)->rf

#endif

#define c(tn)	(tn)->c
#define p(tn)	(tn)->p
#define l(tn)	(tn)->l
#define r(tn)	(tn)->r

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

static inline void addNewTnode(tnode *newNode,tnode *parent,char lr)
{
	if((lr != TNODE_RC && lr != TNODE_LC) || !parent || !newNode) {
		fprintf(stderr,"invalid arg!\n");
		return;
	}

//	if(!parent) {
//		if (root) {
//			fprintf(stderr,"root has been set,please specify parent node!\n");
//			return;
//		}
//		root = parent;
//	}
	
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

void inOrderTraverse(tnode *r);
void inOrderTraverseNoRecur(tnode *r);

void preOrderTraverse(tnode *r);
void preOrderTraverseNoRecur(tnode *r);

void postOrderTraverse(tnode *r);
void postOrderTraverseNoRecur(tnode *r);
void postOrderTraverseNoRecur_(tnode *r);

#ifdef THREADING_TREE

void inOrderThreading(tnode *r,tnode *head);
void inOrderTraverseThread(tnode *r);

tnode * postOrderThreading(tnode *r);
void postOrderTraverseThread(tnode *r,tnode *head);

#endif

#endif
