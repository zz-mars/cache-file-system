#include "scc.h"
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
vertices_t *breadth_first_search(adj_list_t *adj_list)
{
	/* initialize vertices */
	int i;
	vertices_t *vt,*start_vt;
	for(i=0;i<adj_list->vts_nr;i++) {
		vt = &adj_list->vts[i];
		vt->color = VT_COLOR_WHITE;
		vt->distance = MAX_INFINITY;
		vt->parent = vt->first_child = vt->next_sibling = NULL;
	}

	/* start from the first vertice */
	start_vt = &adj_list->vts[0];
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
		for(node=adj_list->heads[vt->key-1].next;node!=NULL;node=node->next) {
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

static void dfs_visit(adj_list_t *adj_list,vertices_t *vt,vertices_t **topsorthead)
{
	vt->time.d = ++t;
	vt->color = VT_COLOR_GRAY;

	visit(vt);

	vertices_t *child_vt;
	adj_list_node_t *node;
	for(node=adj_list->heads[vt->key-1].next;node!=NULL;node=node->next) {
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

vertices_t *raw_depth_first_search(adj_list_t *adj_list,vertices_t **topsorthead)
{
	/* initialize before depth_first_search */
	int i;
	vertices_t *vt,*start_vt;
	for(i=0;i<adj_list->vts_nr;i++) {
		vt = &adj_list->vts[i];
		vt->color = VT_COLOR_WHITE;
		vt->distance = MAX_INFINITY;
		vt->parent = vt->first_child = vt->next_sibling = NULL;
		vt->time.d = vt->time.f = 0;
	}

	/* start from the first vertice */
	start_vt = &adj_list->vts[0];
	start_vt->parent = NULL;
	start_vt->distance = 0;

	/* initialize time */
	t = 0;

	for(i=0;i<adj_list->vts_nr;i++) {
		vt = &adj_list->vts[i];
		if(vt->color == VT_COLOR_WHITE) {
			dfs_visit(adj_list,vt,topsorthead);
		}
	}

	printf("\n");

	return start_vt;
}

vertices_t *depth_first_search(adj_list_t *adj_list)
{
	return raw_depth_first_search(adj_list,NULL);
}

vertices_t *topological_sort(adj_list_t *adj_list)
{
	vertices_t *tophead = NULL;
	raw_depth_first_search(adj_list,&tophead);
	return tophead;
}

