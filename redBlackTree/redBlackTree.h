#ifndef _RBT_H
#define _RBT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

enum {
	RBT_RED = 'R',
	RBT_BLACK = 'B'
};

typedef struct rb_node_t{
	int i;
	char c;
	struct rb_node_t * l;
	struct rb_node_t * r;
	struct rb_node_t * p;
	void *data;
}rb_node_t;

typedef struct {
	rb_node_t *root;
	int nodes_nr;
}redBlackTree_t;

#define p(z)	(z)->p
#define l(z)	(z)->l
#define r(z)	(z)->r
#define c(z)	(z)->c
#define i(z)	(z)->i
#define d(z)	(z)->data

/* NIL_NODE is declared in this header file 
 * and defined in redBlackTree.c */
extern rb_node_t *NIL_NODE;

rb_node_t * rbt_max(rb_node_t * z);
rb_node_t * rbt_min(rb_node_t * z);

rb_node_t * rbt_suc(rb_node_t * z);
rb_node_t * rbt_pre(rb_node_t * z);

/* redBlackTree_t that these functions will operate on must be specified */
void rbt_init(redBlackTree_t *T);
void rbt_destory(redBlackTree_t *T);

rb_node_t * search_node(redBlackTree_t *T,int key);

void rbt_insert(redBlackTree_t *T,int key);
rb_node_t * rbt_delete(redBlackTree_t *T,int key);

void printRedBlackTree(redBlackTree_t *T);

static inline rb_node_t * new_rb_node(int key)
{
	rb_node_t * new_node = (rb_node_t *)malloc(sizeof(rb_node_t));
	if(new_node == NULL){
		perror("new rb_node_t");
		return NIL_NODE;
	}

	i(new_node) = key;
	l(new_node) = NIL_NODE;
	r(new_node) = NIL_NODE;
	p(new_node) = NIL_NODE;
	c(new_node) = RBT_RED;

	return new_node;
}

enum {
	LEFT_CHILD = 1,		
	PARENT,		
	RIGHT_CHILD		
};

static inline void print_rb_node(rb_node_t * node,char i)
{
	if(node == NIL_NODE){
		fprintf(stderr,"NULL NODE!\n");
		return;
	}

	char *lpr;
	switch(i){
		case LEFT_CHILD:
			lpr = "left-child";
			break;
		case PARENT:
			lpr = "parent";
			break;
		case RIGHT_CHILD:
			lpr = "right-child";
			break;
	}
	printf("%s : %3d : %c\n",lpr,i(node),c(node));
}

#endif
