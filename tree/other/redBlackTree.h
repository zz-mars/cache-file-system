#ifndef _RBT_H
#define _RBT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define RBT_RED			'R'
#define RBT_BLACK		'B'

#define RED_STR			"red"
#define BLACK_STR		"black"

enum {
	LEFT_CHILD = 1,		
	PARENT,		
	RIGHT_CHILD		
};

#define H_LEFT_CHILD	"LEFT_CHILD : "
#define H_PARENT		"PARENT : "
#define H_RIGHT_CHILD	"RIGHT_CHILD : "

typedef struct rb_node_t{
	int i;
	char c;
	struct rb_node_t * l;
	struct rb_node_t * r;
	struct rb_node_t * p;
	void *data;
}rb_node_t;

#define p(z)			(z)->p
#define l(z)			(z)->l
#define r(z)			(z)->r
#define c(z)			(z)->c
#define i(z)			(z)->i

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

static inline void free_rb_node(rb_node_t * r)
{
	if(r != NIL_NODE){
		free(r);
	}
}

static inline void free_rbt(rb_node_t *root)
{
	if(root == NIL_NODE) {
		return;
	}
	free_rbt(l(root));
	free_rbt(r(root));
	free_rb_node(root);
}

static inline void print_rb_node(rb_node_t * node,char i)
{
	char * lpr;

	if(node == NIL_NODE){
		fprintf(stderr,"NULL NODE!\n");
		return;
	}

	switch(i){
		case LEFT_CHILD:
			lpr = H_LEFT_CHILD;
			break;
		case PARENT:
			lpr = H_PARENT;
			break;
		case RIGHT_CHILD:
			lpr = H_RIGHT_CHILD;
			break;
		default:
			fprintf(stderr,"UNRECOGNIZED I\n");
			return;
	}
	printf("%s%3d -- %c\n",lpr,i(node),c(node));
}

static inline void print_rbt(rb_node_t * r)
{
	rb_node_t * n = r;

	if(n == NIL_NODE){
		return;
	}
	print_rb_node_t_node(n,PARENT);
	if(l(n) != NIL_NODE){
		print_rb_node_t_node(l(n),LEFT_CHILD);
	}
	if(r(n) != NIL_NODE){
		print_rb_node_t_node(r(n),RIGHT_CHILD);
	}
	printf("------------------------------------\n");
	print_rb_node_t(l(n));
	print_rb_node_t(r(n));
}

#endif
