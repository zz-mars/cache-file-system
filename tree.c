#include"glob.h"
typedef struct TREE_NODE{
	u32 i;
	struct TREE_NODE * p;
	struct TREE_NODE * left;
	struct TREE_NODE * right;
}node;
static u32 a[] = {2,399,387,219,266,382,381,278,363,-1};
static node * root = NULL;
static node * get_node_from_tree(node * root,u32 value,u32 * i)
{
	node * n = root;
	if(n == NULL){
		*i = 2;
		fprintf(stderr,"NULL tree!\n");
		return NULL;
	}
	while(1){
		if(n->i > value){
			if(!(n->left)){
				*i = -1;
				break;
			}else{
				n = n->left;
			}
		}else if(n->i < value){
			if(!(n->right)){
				*i = 1;
				break;
			}else{
				n = n->right;
			}
		}else{
			*i = 0;
			break;
		}
	}
	return n;
}
static inline node * my_new_node()
{
	node * n = (node *)malloc(sizeof(node));
	if(n == NULL){
		perror("malloc for a new node");
		return NULL;
	}
	bzero(n,sizeof(node));
	n->left = NULL;
	n->right = NULL;
	n->p = NULL;
	n->i = 0;
	return n;
}
static void init_tree()
{
	u32 i,di,j = 0;
	node * n,*m;
new_node:
	/*
	printf("input node index:\n");
	scanf("%d",&i);
	*/
	i = a[j++];
	if(i != -1){
		n = my_new_node();
		n->i = i;
		m = get_node_from_tree(root,i,&di);
		if(m == NULL){root = n;}
		else{
			printf("get_node_from_tree return node -- %d\n",m->i);
			printf("di -- %d\n",di);
			switch(di){
				case -1:
					m->left = n;
					n->p = m;
					break;
				case 0:
					/* not defined */
					break;
				case 1:
					m->right = n;
					n->p = m;
					break;
				default:
					break;
			}
		}
		goto new_node;
	}
	return;
}
static void print_tree(node * r)
{
	node * n = r;
	if(n == NULL){
	//	printf("no node to print!\n");
		return;
	}
	printf("p -- %d\n",n->i);
	if(n->left){
		printf("l -- %d\t",n->left->i);
	}
	if(n->right){
		printf("r -- %d",n->right->i);
	}
	printf("\n--------------------------------------------\n");
	print_tree(n->left);
	print_tree(n->right);
	return;
}
static node * tree_max(node * r)
{
	node * n = r;
	if(n == NULL){
		return NULL;
	}
	while(n->right != NULL){n = n->right;}
	return n;
}
static node * tree_min(node * r)
{
	node * n = r;
	if(n == NULL){
		return NULL;
	}
	while(n->left != NULL){n = n->left;}
	return n;
}
static node * tree_max_rec(node *r)
{
	if(r == NULL){
		fprintf(stderr,"cannot calculate max for a null tree!\n");
		return NULL;
	}
	if(r->right){
		return tree_max(r->right);
	}else{
		return r;
	}
}
static node * tree_min_rec(node *r)
{
	if(r == NULL){
		fprintf(stderr,"cannot calculate min for a null tree!\n");
		return NULL;
	}
	if(r->left){
		return tree_min_rec(r->left);
	}else{
		return r;
	}
}
static node * pre_node(const u32 key)
{
	u32 di;
	node * p;
	node * n = get_node_from_tree(root,key,&di);
	if(n != NULL && di == 0){
	//	printf("pre_node get_node_from_tree ok!\n");
	}else{return NULL;}
	if(n->left != NULL){
		return tree_max(n->left);
	}
	p = n->p;
	while(p != NULL && (p->right == NULL || n->i != p->right->i)){
		n = p;
		p = n->p;
	}
	return p;
}
static node * suc_node(const u32 key)
{
	u32 di;
	node * p;
	node * n = get_node_from_tree(root,key,&di);
	if(n != NULL && di == 0){
	//	printf("suc_node get_node_from_tree ok!\n");
	}else{return NULL;}
	if(n->right){
		return tree_min(n->right);
	}
	p = n->p;
	while(p != NULL && (p->left == NULL || n->i != p->left->i)){
		n = p;
		p = n->p;
	}
	return p;
}
static u32 rm_node(u32 key)
{
	u32 di;
	node * p;
	node * y = NULL,*x = NULL;
	node * n = get_node_from_tree(root,key,&di);
	if(n == NULL || di != 0){
		fprintf(stderr,"node not exist!\n");
		return 1;
	}
	if(n->left == NULL || n->right == NULL){
		y = n;
	}else{
		y = suc_node(key);
	}
	if(y != NULL && y->left != NULL){
		x = y->left;
	}else if(y != NULL && y->right != NULL){
		x = y->right;
	}
	if(x != NULL){
		x->p = y->p;
	}
	if(y->p == NULL){
		root = x;
	}else{
		if(y == y->p->left){
			y->p->left = x;
		}else if(y == y->p->right){
			y->p->right = x;
		}
	}
	if(y != n){
		n->i = y->i;
	}
	free(y);
	return 0;
}
static void tree_insert(node * r,node * new_node)
{
	node * n = r;
	if(new_node->i < r->i){
		if(r->left == NULL){
			r->left = new_node;
			new_node->p = r;
		}else{
			tree_insert(r->left,new_node);
		}
	}else if(new_node->i >= r->i){
		if(r->right == NULL){
			r->right = new_node;
			new_node->p = r;
		}else{
			tree_insert(r->right,new_node);
		}
	}
}
static void in_order_traverse(node * r)
{
	node * n = r;
	if(n == NULL){
		return;
	}
	if(n->left){
		in_order_traverse(n->left);
	}
	printf("%d ",n->i);
	if(n->right){
		in_order_traverse(n->right);
	}
	return;
}
int main()
{
	u32 i;
	node * m,*n;
	init_tree();
	print_tree(root);
	printf("------------------->360\n");
	n = my_new_node();
	n->i = 360;
	tree_insert(root,n);
	print_tree(root);
	printf("------------------->364\n");
	n = my_new_node();
	n->i = 364;
	tree_insert(root,n);
	print_tree(root);
	printf("-------------------------rm 363\n");
	rm_node(363);
	print_tree(root);
	printf("in order traverse-----------------------------------\n");
	in_order_traverse(root);
	printf("\n");
//	m = tree_max(root);
	m = tree_max_rec(root);
	printf("tree-max -- %d\n",m->i);
//	n = tree_min(root);
	n = tree_min_rec(root);
	printf("tree-min -- %d\n",n->i);
	/*
	i = 219;
	m = suc_node(i);
	if(m){
		printf("suc_node of 219 -- %d\n",m->i);
	}
	*/
	for(i = 0;i<9;i++){
		m = pre_node(a[i]);
		n = suc_node(a[i]);
		if(m == NULL){
			printf("no pre for node %d\n",a[i]);
		}else{
			printf("pre_node of %4d -- %d\n",a[i],m->i);
		}
		if(n == NULL){
			printf("no suc for node %d\n",a[i]);
		}else{
			printf("suc_node of %4d -- %d\n",a[i],n->i);
		}
		printf("-------------------------------------------------------------\n");
	}
	return 0;
}
