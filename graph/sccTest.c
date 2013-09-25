#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "scc.h"

int main()
{
	adj_list_t *adj_list = build_adj_list();
	if(!adj_list) {
		return 1;
	}

	print_adj_list(adj_list);

	printf("------------- breadth first search -------------\n");
	vertices_t *root = breadth_first_search(adj_list);
	if(root){
		printf("------------- breadth first search tree -------------\n");
		print_search_tree(root);
	}else {
		fprintf(stderr,"breadth_first_search fail!\n");
	}

	printf("------------- depth first search -------------\n");
	root = depth_first_search(adj_list);
	if(root){
		printf("------------- depth first search tree -------------\n");
		print_search_tree(root);
	}else {
		fprintf(stderr,"depth_first_search fail!\n");
	}

	printf("------------- topological_sort -------------\n");
	root = topological_sort(adj_list);
	while(root) {
		printf("%d ",root->key);
		root = root->next;
	}
	printf("\n");

	destroy_adj_list(adj_list);

	return 0;
}
