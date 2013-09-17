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

#define LEVEL_PRINT

typedef struct rb_node_t{
	int i;
	char c;
	struct rb_node_t * l;
	struct rb_node_t * r;
	struct rb_node_t * p;
	void *data;
#ifdef LEVEL_PRINT
	char last_in_this_level;
#endif
}rb_node_t;

typedef struct {
	rb_node_t *root;
	int nodes_nr;
}redBlackTree_t;

extern rb_node_t *NIL_NODE;

void rbt_init(redBlackTree_t *T);
void rbt_destory(redBlackTree_t *T);

rb_node_t * search_node(redBlackTree_t *T,int key);

void rbt_insert(redBlackTree_t *T,int key);
rb_node_t * rbt_delete(redBlackTree_t *T,int key);

#ifdef LEVEL_PRINT
void rbt_level_traverse(redBlackTree_t *T);
#endif

#endif

