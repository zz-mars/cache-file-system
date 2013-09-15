#include "redBlackTree.h"

static rb_node_t nil_node;

rb_node_t *NIL_NODE = NULL;

void rbt_init(redBlackTree_t *T)
{
	T->root = NIL_NODE = &nil_node;
	c(NIL_NODE) = RBT_BLACK;
	T->nodes_nr = 0;
}

/* free the whole subtree */
static void free_rbt(rb_node_t *root)
{
	if(root == NIL_NODE) {
		return;
	}
	free_rbt(l(root));
	free_rbt(r(root));
	free(root);
}

void rbt_destory(redBlackTree_t *T)
{
	free_rbt(T->root);
}

static void print_rbt(rb_node_t * r)
{
	rb_node_t * n = r;

	if(n == NIL_NODE){
		return;
	}
	print_rb_node(n,PARENT);
	if(l(n) != NIL_NODE){
		print_rb_node(l(n),LEFT_CHILD);
	}
	if(r(n) != NIL_NODE){
		print_rb_node(r(n),RIGHT_CHILD);
	}
	printf("------------------------------------\n");
	print_rbt(l(n));
	print_rbt(r(n));
}

void printRedBlackTree(redBlackTree_t *T)
{
	print_rbt(T->root);
}

enum {
	RAW_SEARCH_NULL_TREE = 0,
	RAW_SEARCH_LEFT_CHILD,
	RAW_SEARCH_RIGHT_CHILD,
	RAW_SEARCH_EXIST
};
/* raw_search_node : seach node for a given key */
static rb_node_t * raw_search_node(redBlackTree_t *T,int key,int * di)
{
	rb_node_t * node = T->root;

	*di = RAW_SEARCH_NULL_TREE;

	while(node != NIL_NODE){
		if(key == i(node)) {
			*di = RAW_SEARCH_EXIST;
			break;
		}

		if(key < i(node)){
			if(l(node) == NIL_NODE) {
				*di = RAW_SEARCH_LEFT_CHILD;
				break;
			}
			node = l(node);
		}else if(key > i(node)){
			if(r(node) == NIL_NODE) {
				*di = RAW_SEARCH_RIGHT_CHILD;
				break;
			}
			node = r(node);
		}
	}

	return node;
}

/* search_node : search a node for a given key
 * if node is found,its pointer is returned
 * else NIL_NODE is returned */
rb_node_t * search_node(redBlackTree_t *T,int key)
{
	int di;
	rb_node_t *node = raw_search_node(T,key,&di);

	if(di != RAW_SEARCH_EXIST) {
		node = NIL_NODE;
	}

	return node;
}

rb_node_t * rbt_max(rb_node_t * z)
{
	if(z == NIL_NODE) {
		return NIL_NODE;
	}

	while(r(z) != NIL_NODE){z = r(z);}

	return z;
}

rb_node_t * rbt_min(rb_node_t * z)
{
	if(z == NIL_NODE) {
		return NIL_NODE;
	}

	while(r(z) != NIL_NODE){z = l(z);}

	return z;
}

rb_node_t * rbt_suc(rb_node_t * z)
{
	if(z == NIL_NODE){
		return NIL_NODE;
	}

	if(r(z) != NIL_NODE){
		return rbt_min(r(z));
	}

	rb_node_t *x = z;
	while(p(x) != NIL_NODE && x == r(p(x))){
		x = p(x);
	}
	return p(x);
}

rb_node_t * rbt_pre(rb_node_t * z)
{
	if(z == NIL_NODE){
		return z;
	}

	if(l(z) != NIL_NODE){
		return rbt_max(l(z));
	}

	rb_node_t *x = z;
	while(p(x) != NIL_NODE && x == l(p(x))){
		x = p(x);
	}
	return p(x);
}

static rb_node_t * rbt_simple_insert(redBlackTree_t *T,int key)
{
	int di;
	rb_node_t *node = raw_search_node(T,key,&di);
	if(di == RAW_SEARCH_EXIST) {
		fprintf(stderr,"key %d already exist!\n",key);
		return NIL_NODE;
	}

	rb_node_t *newNode = new_rb_node(key);
	if(newNode == NIL_NODE) {
		return NIL_NODE;
	}

	p(newNode) = node;

	if(di == RAW_SEARCH_LEFT_CHILD) {
		l(node) = newNode;
	}else if(di == RAW_SEARCH_RIGHT_CHILD) {
		r(node) = newNode;
	}else if(di == RAW_SEARCH_NULL_TREE) {
		T->root = newNode;
	}

	return newNode;
}

static void left_rotate(redBlackTree_t *T,rb_node_t * x)
{
	rb_node_t * y = r(x);

	if(y == NIL_NODE) {
		fprintf(stderr,"right child is NIL_NODE,cannot left-rotate!\n");
		return;
	}

	r(x) = l(y);
	if(l(y) != NIL_NODE){
		p(l(y)) = x;
	}

	p(y) = p(x);
	if(p(x) == NIL_NODE){
		T->root = y;
	}else{
		if(l(p(x)) == x){
			l(p(x)) = y;
		}else if(r(p(x)) == x){
			r(p(x)) = y;
		}
	}

	l(y) = x;
	p(x) = y;
}

static void right_rotate(redBlackTree_t *T,rb_node_t * x)
{
	rb_node_t * y = l(x);
	if(y == NIL_NODE) {
		fprintf(stderr,"left child is NIL_NODE,cannot right-rotate!\n");
		return;
	}

	l(x) = r(y);
	if(r(y) != NIL_NODE){
		p(r(y)) = x;
	}

	p(y) = p(x);
	if(p(x) == NIL_NODE){
		T->root = y;
	}else{
		if(l(p(x)) == x){
			l(p(x)) = y;
		}else if(r(p(x)) == x){
			r(p(x)) = y;
		}
	}

	r(y) = x;
	p(x) = y;
}

/* As we add a new node with color red,
 * if color of its parent is red too,
 * we need to resolve the additional red color.
 * We make it by considering the following 3 cases :
 *
 * 1. When z's uncle is red too,so both z's parent and z's uncle is red.
 * In this case, we simply color z's parent and z's uncle to black,
 * and color z's grand-parent from black to red.
 * Now there's no violation between z and its parent.
 * The node that may cause violation goes to z's grand-parent,
 * who is colored from black to red.So we start a new loop with z=p(p(z)). 
 *
 * 2. If z's uncle is black,we cannot simply do as the case above.
 * Considering our goal to resolve the additional red color,
 * we may make it by some rotation and the red color will finally go
 * to z's uncle's side.As z's uncle is black,it will be okay for its uncle.
 * Mention that we may need more than one rotation to make it.
 * Here we only consider the case of p(z) == l(p(p(z))),
 * which is symmetric with the case of p(z) == r(p(p(z))).
 * Now we only condiser the first case,and there are two subcases for each one:
 *
 *	2.1 When z is right child of its parent ( z == r(p(z)) ).
 *	We do the following :
 *		z = p(z);
 *		left-rotate(T,z);
 *	After then,z and p(z) will still be red,which is still violating the rule.
 *	But z now becomes left child of its parent,which is next case we will consider.
 *
 *	2.2 When z is left child of its parent ( z == l(p(z)) ).
 *	We do the following :
 *		c(p(z)) = RBT_BLACK;
 *		c(p(p(z))) = RBT_RED;
 *		right_rotate(T,p(p(z)));
 *	We paint p(z) to black and p(p(z)) to red,and right rotate p(p(z)),
 *	after then the additional red color goes to z's uncle's side and
 *	the black-height property stay unchanged.
 *
 * PS:ROTATE WILL NOT CHANGE THE BLACK-HEIGHT PROPERTY.
 *	*/
static void rbt_insert_fixup(redBlackTree_t *T,rb_node_t * z)
{
	rb_node_t * y;
	while(c(p(z)) == RBT_RED){
		/* p(z) not null and color is red,
		 * so p(z) is not root --> p(p(z)) exists
		 * and c(p(p(z))) is RBT_BLACK */
		if(p(z) == l(p(p(z)))){
			/* p(z) is left child of its parent */
			y = r(p(p(z)));
			/*------------- case 1 --------------*/
			if(c(y) == RBT_RED){
				/* p(z) 's parent must be black! */
				c(y) = RBT_BLACK;
				c(p(z)) = RBT_BLACK;
				c(p(p(z))) = RBT_RED;
				z = p(p(z));
			/*------------- case 2 --------------*/
			}else if(z == r(p(z))){
				/* y == NIL_NODE or c(y) is black */
				z = p(z);
				left_rotate(T,z);
			/*------------- case 3 --------------*/
			}else{
				/* z is left child of its parent */
				c(p(z)) = RBT_BLACK;
				c(p(p(z))) = RBT_RED;
				right_rotate(T,p(p(z)));
			}
		}else{
			/* p(z) is right child of its parent */
			y = l(p(p(z)));
			if(c(y) == RBT_RED){
				c(y) = RBT_BLACK;
				c(p(z)) = RBT_BLACK;
				c(p(p(z))) = RBT_RED;
				z = p(p(z));
			}else if(z == l(p(z))){
				z = p(z);
				right_rotate(T,z);
			}else{
				c(p(z)) = RBT_BLACK;
				c(p(p(z))) = RBT_RED;
				left_rotate(T,p(p(z)));
			}
		}
	}
	c(T->root) = RBT_BLACK;
}

void rbt_insert(redBlackTree_t *T,int key)
{
	rb_node_t * z = rbt_simple_insert(T,key);

	if( z == NIL_NODE) {
		return;
	}

	rbt_insert_fixup(T,z);
}

static void rbt_delete_fixup(redBlackTree_t *T,rb_node_t * z)
{
	rb_node_t * x = z,*w;
	while(x != T->root && c(x) == RBT_BLACK){
		if(x == l(p(x))){
			w = r(p(x));
			/*-------- case 1 --------*/
			if(c(w) == RBT_RED){
				/* brother of x is red,
				 * so c(p(x)) is black,
				 * repaint and rotate to give x a brother whose color is black */
				c(w) = RBT_BLACK;
				c(p(x)) = RBT_RED;
				left_rotate(T,p(x));
				w = r(p(x));
			/*-------- case 2 --------*/
			}else if(c(l(w)) == RBT_BLACK && c(r(w)) == RBT_BLACK){
				/* color of w is black,
				 * and two children are all black. 
				 * ready to move black color from x&w to their parent.*/
				c(w) = RBT_RED;
				x = p(x);
			/*-------- case 3 --------*/
			}else if(c(r(w)) == RBT_BLACK){
				/* w is black,
				 * right child of w is black,
				 * left child is red. */
				c(l(w)) = RBT_BLACK;
				c(w) = RBT_RED;
				right_rotate(T,w);
				w = r(p(x));
			/*-------- case 4 --------*/
			}else{
				/* right child of w is red,
				 * w is black. */
				c(w) = c(p(x));
				c(p(x)) = RBT_BLACK;
				c(r(w)) = RBT_BLACK;
				left_rotate(T,p(x));
				x = T->root;
			}
		}else if(x == r(p(x))){
			w = l(p(x));
			if(c(w) == RBT_RED){
				c(w) = RBT_BLACK;
				c(p(x)) = RBT_RED;
				right_rotate(T,p(x));
				w = l(p(x));
			}else if(c(l(w)) == RBT_BLACK && c(r(w)) == RBT_BLACK){
				c(w) = RBT_RED;
				x = p(x);
			}else if(c(l(w)) == RBT_BLACK){
				/* right child of w is red */
				c(w) = RBT_RED;
				c(r(w)) = RBT_BLACK;
				left_rotate(T,w);
				w = l(p(x));
			}else{
				/* left child of w is red */
				c(w) = c(p(x));
				c(l(w)) = RBT_BLACK;
				c(p(x)) = RBT_BLACK;
				right_rotate(T,p(x));
				x = T->root;
			}
		}
	}
	c(x) = RBT_BLACK;
}

/* transplant the whole subtree 'old' by 'new' */
static void rb_transplant(redBlackTree_t *T,rb_node_t *old,rb_node_t *new)
{
	if(!old || old == NIL_NODE) {
		fprintf(stderr,"invalid arguments!\n");
		return;
	}

	if(p(old) == NIL_NODE) {
		T->root = new;
	}else if(old == l(p(old))) {
		l(p(old)) = new;
	}else if(old == r(p(old))) {
		r(p(old)) = new;
	}

	p(new) = p(old);
}

static int rbt_simple_delete(redBlackTree_t *T,int key,
		rb_node_t **to_delete,rb_node_t **to_fixup)
{
	rb_node_t *node = search_node(T,key);
	if(node == NIL_NODE) {
		return 1;
	}

	rb_node_t *to_del = node;
	rb_node_t *to_fix;
	if(l(node) == NIL_NODE) {
		to_fix = r(node);
	}else if(r(node) == NIL_NODE) {
		to_fix = l(node);
	}else {
		to_del = rbt_min(r(node));
		to_fix = r(to_del);

		/* when 'node' is not the node to be deleted 
		 * exchange key and satalite data */
		int i = i(node);
		i(node) = i(to_del);
		i(to_del) = i;

		char *data = d(node);
		d(node) = d(to_del);
		d(to_del) = data;
	}

	rb_transplant(T,to_del,to_fix);
	*to_delete = to_del;
	*to_fixup = to_fix;

	return 0;
}

rb_node_t * rbt_delete(redBlackTree_t *T,int key)
{
	rb_node_t *to_del,*to_fix;

	if(rbt_simple_delete(T,key,&to_del,&to_fix)) {
		return NIL_NODE;
	}

	if(c(to_del) == RBT_BLACK) {
		rbt_delete_fixup(T,to_fix);
	}

	return to_del;
}

/* old version of rbt_delete */
//rb_node_t * rbt_delete(redBlackTree_t *T,int key)
//{
//	int di;
//	rb_node_t *x,*y;
//	rb_node_t * z = raw_search_node(T,key,&di);
//	if(z == NIL_NODE || di != 0){
//		fprintf(stderr,"NO NODE WITH KEY %d\n",key);
//		return NIL_NODE;
//	}
//	/* z != NIL_NODE && di == 0 */
//	/* y is the node to be deleted */
//	if(l(z) == NIL_NODE || r(z) == NIL_NODE){
//		y = z;
//	}else{
//		y = rbt_suc(z);
//	}
//	if(l(y) != NIL_NODE){
//		x = l(y);
//	}else{
//		x = r(y);
//	}
//	p(x) = p(y);
//	if(p(y) == NIL_NODE){
//		T->root = x;
//	}else{
//		if(y == l(p(y))){
//			l(p(y)) = x;
//		}else if(y == r(p(y))){
//			r(p(y)) = x;
//		}
//	}
//	if(y != z){
//		i(z) = i(y);
//	}
//	if(c(y) == RBT_BLACK){
//		rbt_delete_fixup(T,x);
//	}
//	return y;
//}

