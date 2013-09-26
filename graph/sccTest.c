#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "scc.h"

int main()
{
	graph_t *g = build_graph_from_input();
	if(!g) {
		return 1;
	}

	print_graph(g);

	printf("------------- breadth first search -------------\n");
	vertices_t *root = breadth_first_search(g);
	if(root){
		printf("------------- breadth first search tree -------------\n");
		print_search_tree(root);
	}else {
		fprintf(stderr,"breadth_first_search fail!\n");
	}

	printf("------------- depth first search -------------\n");
	root = depth_first_search(g);
	if(root){
		printf("------------- depth first search tree -------------\n");
		print_search_tree(root);
	}else {
		fprintf(stderr,"depth_first_search fail!\n");
	}

	printf("------------- topological_sort -------------\n");
	root = topological_sort(g);
	while(root) {
		printf("%d ",root->key);
		root = root->next;
	}
	printf("\n");

	printf("------------- transpose_graph -------------\n");
	if(transpose_graph(g)) {
		fprintf(stderr,"transpose_graph fail!\n");
	} else {
		print_graph(g);
	}

	destroy_graph(g);

	return 0;
}
