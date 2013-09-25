#ifndef _SCC_H
#define _SCC_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

typedef struct time_stamps_t {
	int d;
	int f;
} time_stamps_t;

typedef struct vertices_t {
	int key;
	char color;
	int distance;
	struct vertices_t *parent;
	struct vertices_t *first_child;
	struct vertices_t *next_sibling;
	time_stamps_t time;
	struct vertices_t *next;	//for topological sort
} vertices_t;

typedef struct adj_list_node_t {
	vertices_t *vertice;
	struct adj_list_node_t *next;
} adj_list_node_t;

typedef struct adj_list_t {
	int vts_nr;
	vertices_t *vts;
	adj_list_node_t *heads;
} adj_list_t;

adj_list_t *build_adj_list(void);

void destroy_adj_list(adj_list_t *adj_list);

void print_adj_list(adj_list_t *adj_list);

vertices_t *breadth_first_search(adj_list_t *adj_list);

vertices_t *depth_first_search(adj_list_t *adj_list);

void print_search_tree(vertices_t *root);

vertices_t *topological_sort(adj_list_t *adj_list);

#endif

