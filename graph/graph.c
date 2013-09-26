#include "graph.h"
#include "queue.h"
#include <assert.h>

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

void print_graph(graph_t *g)
{
	printf("---------- adjacent list basic infomation ----------\n");
	printf("vertice_nr #%d\n",g->vts_nr);
	int i;
	for(i=0;i<g->vts_nr;i++) {
		adj_list_node_t *p = &g->adj_list[i];
		printf("#");
		while(p) {
			printf("%2d - ",p->vertice->key);
			p = p->next;
		}
		printf("\n");
	}
}

/* @destroy_adj_list : destroy the adj_list of a graph */
static inline void destroy_adj_list(adj_list_node_t **adj_list_pp,int vts_nr)
{
	adj_list_node_t *adj_list = *adj_list_pp;
	if(!adj_list) {
		return;
	}

	*adj_list_pp = NULL;

	int i;
	for(i=0;i<vts_nr;i++) {
		adj_list_node_t	*n = adj_list[i].next;
		while(n) {
			/* get next node before free this one */
			adj_list_node_t *next = n->next;
			destroy_adj_list_node(n);
			n = next;
		}
	}

	free(adj_list);
}

static inline adj_list_node_t *init_adj_list(vertices_t *vts,int vts_nr)
{
	adj_list_node_t *adj_list = (adj_list_node_t*)malloc(sizeof(adj_list_node_t)*vts_nr);
	if(!adj_list) {
		perror("malloc");
		return NULL;
	}

	/* initialize the heads */
	int i;
	for(i=0;i<vts_nr;i++) {
		adj_list[i].vertice = &vts[i];
		adj_list[i].next = NULL;
	}

	return adj_list;
}

void destroy_graph(graph_t *g)
{
	if(!g) {
		return;
	}
	 
	if(g->vts) {
		free(g->vts);
	}

	destroy_adj_list(&g->adj_list,g->vts_nr);

	free(g);
}

graph_t *build_graph_from_input(void)
{
	int vts_nr;
	scanf("%d",&vts_nr);

	if(vts_nr <= 0) {
		fprintf(stderr,"invalid arg!\n");
		return NULL;
	}

	graph_t *g = (graph_t*)malloc(sizeof(graph_t));
	if(!g) {
		perror("malloc");
		return NULL;
	}

	g->vts_nr = vts_nr;
	g->adj_list = NULL;

	g->vts = (vertices_t*)malloc(sizeof(vertices_t)*vts_nr);
	if(!g->vts) {
		perror("malloc");
		goto err_lb1;
	}

	int i;
	for(i=0;i<vts_nr;i++) {
		g->vts[i].key = i+1;
	}

	g->adj_list = init_adj_list(g->vts,g->vts_nr);
	if(!g->adj_list) {
		fprintf(stderr,"init_adj_list fail!\n");
		goto err_lb1;
	}

	/* make the adj list */
	int m,n;
	scanf("%d%d",&m,&n);
	while(!(m==0 && n==0)) {
		/* check if out of range */
		if(m > vts_nr || n > vts_nr) {
			goto err_lb1;
		}

		/* adjust index */
		m--;
		n--;

		adj_list_node_t *p = new_adj_list_node(&g->vts[n]);
		if(!p) {
			goto err_lb1;
		}

		p->next = g->adj_list[m].next;
		g->adj_list[m].next = p;
		scanf("%d%d",&m,&n);
	}

	return g;

	/* exit gracefully */
err_lb1:
	destroy_graph(g);
	return NULL;
}

int transpose_graph(graph_t *g)
{
	int vts_nr = g->vts_nr;

	adj_list_node_t *adj_list = init_adj_list(g->vts,vts_nr);
	if(!adj_list) {
		fprintf(stderr,"init_adj_list fail!\n");
		return 1;
	}

	int i;
	for(i=0;i<vts_nr;i++) {
		adj_list_node_t *n = g->adj_list[i].next;
		while(n) {
			int j = n->vertice->key-1;
			adj_list_node_t *m = new_adj_list_node(&g->vts[i]);
			if(!m) {
				goto err_lb1;
			}

			m->next = adj_list[j].next;
			adj_list[j].next = m;

			n = n->next;
		}
	}

	destroy_adj_list(&g->adj_list,vts_nr);
	g->adj_list = adj_list;
	return 0;

	/* exit gracefully */
err_lb1:
	destroy_adj_list(&adj_list,vts_nr);

	return 1;
}

#define VT_COLOR_WHITE	'W'
#define VT_COLOR_GRAY	'G'
#define VT_COLOR_BLACK	'B'

#define MAX_INFINITY	1000000

#define visit(vt)	do {	\
	vertices_t *_vt = vt;	\
	printf("%d ",_vt->key);	\
} while(0)

/* breadth-first search */
/* vertices color represents its state :
 * WHITE : not discovered
 * GRAY  : discoverd,now in queue
 * BLACK : visied,now out of queue 
 * */
vertices_t *breadth_first_search(graph_t *g)
{
	adj_list_node_t *adj_list = g->adj_list;

	/* initialize vertices */
	int i;
	vertices_t *vt,*start_vt;
	for(i=0;i<g->vts_nr;i++) {
		vt = &g->vts[i];
		vt->color = VT_COLOR_WHITE;
		vt->distance = MAX_INFINITY;
		vt->parent = vt->first_child = vt->next_sibling = NULL;
	}

	/* start from the first vertice */
	start_vt = &g->vts[0];
	start_vt->parent = NULL;
	start_vt->distance = 0;
	
	queue_t *q = init_queue(INIT_QUEUE_SZ);

	if(!q) {
		return NULL;
	}

	start_vt->color = VT_COLOR_GRAY;
	if(en_queue(q,(void*)start_vt)) {
		start_vt = NULL;
		goto ret;
	}

	while(!queue_empty(q)) {
		de_queue(q,(void**)&vt);
		assert(vt->color == VT_COLOR_GRAY);

		/* visit */
		visit(vt);
		adj_list_node_t *node;
		for(node=g->adj_list[vt->key-1].next;node!=NULL;node=node->next) {
			vertices_t *child_vt = node->vertice;
			if(child_vt->color == VT_COLOR_WHITE) {

				child_vt->color = VT_COLOR_GRAY;

				if(en_queue(q,(void*)child_vt)) {
					start_vt = NULL;
					goto ret;
				}

				child_vt->distance = vt->distance + 1;
				child_vt->parent = vt;
				child_vt->next_sibling = vt->first_child;
				vt->first_child = child_vt;
			}
		}

		vt->color = VT_COLOR_BLACK;
	}
	printf("\n");

ret:
	destory_queue(q);

	return start_vt;
}

void print_search_tree(vertices_t *root)
{
	if(!root) {
		return;
	}

	queue_t *q = init_queue(INIT_QUEUE_SZ);
	if(!q) {
		return;
	}

	vertices_t *vt = root;

	if(en_queue(q,(void*)vt)) {
		goto ret;
	}

	while(!queue_empty(q)) {
		de_queue(q,(void**)&vt);
		printf("parent #%d^%d^%d children - ",vt->time.d,vt->key,vt->time.f);
		for(vt=vt->first_child;vt!=NULL;vt=vt->next_sibling) {
			if(en_queue(q,(void*)vt)) {
				goto ret;
			}
			printf("%d^%d^%d - ",vt->time.d,vt->key,vt->time.f);
		}
		printf("\n");
	}
ret:
	destory_queue(q);
}

/* depth first search */
int t;

static void dfs_visit(adj_list_node_t *adj_list,vertices_t *vt,vertices_t **topsorthead)
{
	vt->time.d = ++t;
	vt->color = VT_COLOR_GRAY;

	if(!topsorthead) {
		visit(vt);
	}

	vertices_t *child_vt;
	adj_list_node_t *node;
	for(node=adj_list[vt->key-1].next;node!=NULL;node=node->next) {
		child_vt = node->vertice;
		if(child_vt->color == VT_COLOR_WHITE) {
			child_vt->distance = vt->distance + 1;
			child_vt->parent = vt;
			child_vt->next_sibling = vt->first_child;
			vt->first_child = child_vt;

			dfs_visit(adj_list,child_vt,topsorthead);
		}
	}

	vt->color = VT_COLOR_BLACK;
	vt->time.f = ++t;

	/* for topological sort */
	if(topsorthead) {
		vt->next = *topsorthead;
		*topsorthead = vt;
	}
}

vertices_t *raw_depth_first_search(graph_t *g,vertices_t **topsorthead)
{
	adj_list_node_t *adj_list = g->adj_list;

	/* initialize before depth_first_search */
	int i;
	vertices_t *vt,*start_vt;
	for(i=0;i<g->vts_nr;i++) {
		vt = &g->vts[i];
		vt->color = VT_COLOR_WHITE;
		vt->distance = MAX_INFINITY;
		vt->parent = vt->first_child = vt->next_sibling = NULL;
		vt->time.d = vt->time.f = 0;
	}

	/* start from the first vertice */
	start_vt = &g->vts[0];
	start_vt->parent = NULL;
	start_vt->distance = 0;

	/* initialize time */
	t = 0;

	for(i=0;i<g->vts_nr;i++) {
		vt = &g->vts[i];
		if(vt->color == VT_COLOR_WHITE) {
			dfs_visit(adj_list,vt,topsorthead);
		}
	}

	printf("\n");

	return start_vt;
}

vertices_t *depth_first_search(graph_t *g)
{
	return raw_depth_first_search(g,NULL);
}

vertices_t *topological_sort(graph_t *g)
{
	vertices_t *tophead = NULL;
	raw_depth_first_search(g,&tophead);
	return tophead;
}

void print_strong_connection_component(graph_t *g)
{
	topological_sort(g);

	int *key = (int*)malloc(sizeof(int)*g->vts_nr);
	if(!key) {
		perror("malloc");
		return;
	}

	int i;
	for(i=0;i<g->vts_nr;i++) {
		key[i] = i;
		g->vts[i].color = VT_COLOR_WHITE;
	}

	/* bubble-sort according to finish time */
	for(i=0;i<g->vts_nr;i++) {
		int j;
		for(j=g->vts_nr-1;j>i;j--) {
			if(g->vts[key[j]].time.f > g->vts[key[j-1]].time.f) {
				key[j] = key[j] ^ key[j-1];
				key[j-1] = key[j] ^ key[j-1];
				key[j] = key[j] ^ key[j-1];
			}
		}
	}

	if(transpose_graph(g)) {
		return;
	}

	/* initialize time */
	t = 0;

	vertices_t *vt;
	for(i=0;i<g->vts_nr;i++) {
		vt = &g->vts[key[i]];
		if(vt->color == VT_COLOR_WHITE) {
			dfs_visit(g->adj_list,vt,NULL);
		}
		printf("\n");
	}

	free(key);
}

