#include "scc.h"
#include <stdlib.h>
#include <unistd.h>

static inline adj_list_node_t *new_adj_list_node(vertices_t *v)
{
	adj_list_node_t *n = (adj_list_node_t*)malloc(sizeof(adj_list_node_t));

	if(!n) {
		perror("malloc");
		return NULL;
	}

	n->next = NULL;
	n->vertice = v;
	return n;
}

static inline void destroy_adj_list_node(adj_list_node_t *n)
{
	if(n) {
		free(n);
	}
}

void print_adj_list(adj_list_t *adj_list)
{
	printf("---------- adjacent list basic infomation ----------\n");
	printf("vertice_nr #%d\n",adj_list->vts_nr);
	int i;
	for(i=0;i<adj_list->vts_nr;i++) {
		adj_list_node_t *p = &adj_list->heads[i];
		printf("#");
		while(p) {
			printf("%2d - ",p->vertice->key);
			p = p->next;
		}
		printf("\n");
	}
}

adj_list_t *build_adj_list(void)
{
	int vts_nr = 8;
	scanf("%d",&vts_nr);
	if(vts_nr <= 0) {
		return NULL;
	}

	/* new adj list */
	adj_list_t *adj_list = (adj_list_t*)malloc(sizeof(adj_list_t));
	if(!adj_list) {
		perror("malloc");
		return NULL;
	}

	adj_list->vts_nr = vts_nr;

	/* vertices */
	adj_list->vts = (vertices_t*)malloc(sizeof(vertices_t)*vts_nr);
	if(!adj_list->vts) {
		perror("malloc");
		goto err_lb1;
	}

	/* initialize vertices key */
	int i;
	for(i=0;i<vts_nr;i++) {
		adj_list->vts[i].key = i+1;
	}

	/* adj list heads */
	adj_list->heads = (adj_list_node_t*)malloc(sizeof(adj_list_node_t)*vts_nr);
	if(!adj_list->heads) {
		perror("malloc");
		goto err_lb2;
	}

	/* initialize the heads */
	for(i=0;i<vts_nr;i++) {
		adj_list->heads[i].vertice = &adj_list->vts[i];
		adj_list->heads[i].next = NULL;
	}

	/* make the adj list */
	int m,n;
	scanf("%d%d",&m,&n);
	while(!(m==0 && n==0)) {
		/* check if out of range */
		if(m > vts_nr || n > vts_nr) {
			goto err_lb3;
		}

		/* adjust index */
		m--;
		n--;

		adj_list_node_t *p = new_adj_list_node(&adj_list->vts[n]);
		if(!p) {
			goto err_lb3;
		}

		p->next = adj_list->heads[m].next;
		adj_list->heads[m].next = p;
		scanf("%d%d",&m,&n);
	}

	return adj_list;

	/* exit gracefully */
err_lb3:
	for(i=0;i<adj_list->vts_nr;i++) {
		adj_list_node_t *p = adj_list->heads[i].next;
		while(p) {
			destroy_adj_list_node(p);
			p = p->next;
		}
	}

	free(adj_list->heads);

err_lb2:
	free(adj_list->vts);

err_lb1:
	free(adj_list);

	return NULL;
}

void destroy_adj_list(adj_list_t *adj_list)
{
	int i;
	for(i=0;i<adj_list->vts_nr;i++) {
		adj_list_node_t *p = adj_list->heads[i].next;
		while(p) {
			destroy_adj_list_node(p);
			p = p->next;
		}
	}

	free(adj_list->heads);

	free(adj_list->vts);

	free(adj_list);
}

