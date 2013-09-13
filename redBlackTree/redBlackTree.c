#include "redBlackTree.h"


static rb_node_t rb_node_nil;

rb_node_t *NIL_NODE;

void rbt_init()
{
	rb_node_nil.c = RBT_BLACK;
	NIL_NODE = &rb_node_nil;
	root = NIL_NODE;
}

/* raw_search_node : seach node for a given key
 * return value : 
 * 1. NIL_NODE is returned when root == NIL_NODE
 *	  and *di is set to 0
 *
 * 2. When none NIL_NODE is returned :
 * 1) *di = 0 if key is found
 * 2) *di = -1 if key is not found 
 *    but should be inserted as left child of node returned
 * 3) *di = 1 if key is not found
 *	  but should be inserted as right child of node */
static rb_node_t * raw_search_node(int key,int * di)
{
	rb_node_t * node = root;

	*di = 0;

	while(node != NIL_NODE){
		if(key == i(node)) {
			break;
		}

		if(key < i(node)){
			if(l(node) == NIL_NODE) {
				*di = -1;
				break;
			}
			node = l(node);
		}else if(key > i(node)){
			if(r(node) == NIL_NODE) {
				*di = 1;
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
rb_node_t * search_node(int key)
{
	int di;
	rb_node_t *node = raw_search_node(key,&di);
	if(di != 0) {
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

int rbt_simple_insert(int key)
{
	int di;
	rb_node_t *node = raw_search_node(key,&di);
	if(di != 0) {
		assert(node != NIL_NODE);
		rb_node_t *newNode = new_rb_node(key);
		if(newNode == NIL_NODE) {
			return 1;
		}
		p(newNode) = node;
		if(di == -1) {
			l(node) = newNode;
		}else if(di == 1) {
			r(node) = newNode;
		}
		return 0;
	}

	if(node == NIL_NODE) {
		root = new_rb_node(key);
		if(root == NIL_NODE) {
			return 1;
		}
		return 0;
	}
	/* key already exist */
	return 1;
}

/* transplant the whole subtree 'old' by 'new' */
static void rbt_transplant(rb_node_t *old,rb_node_t *new)
{
	if(old == NIL_NODE) {
		fprintf(stderr,"invalid arguments!\n");
		return;
	}

	if(p(old) == NIL_NODE) {
		root = new;
	}else if(old == l(p(old))) {
		l(p(old)) = new;
	}else if(old == r(p(old))) {
		r(p(old)) = new;
	}

	if(new != NIL_NODE) {
		p(new) = p(old);
	}
}

void rbt_simple_delete(rb_node_t *node)
{
	if(l(node) == NIL_NODE) {
		rbt_transplant(node,r(node));
	}else if(r(node) == NIL_NODE) {
		rbt_transplant(node,l(node));
	}else {
		rb_node_t *snode = rbt_min(r(node));
		if(snode != r(node)) {
			rbt_transplant(snode,r(snode));
			r(snode) = r(node);
			p(r(node)) = snode;
		}
		l(snode) = l(node);
		p(l(node)) = snode;
		rbt_transplant(node,snode);
	}
}

static void left_rotate(rb_node_t * x)
{
	rb_node_t * y = r(x);
	p(y) = p(x);
	r(x) = l(y);
	if(l(y) != NIL_NODE){
		p(l(y)) = x;
	}
	if(p(x) == NIL_NODE){
		root = y;
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

static void right_rotate(rb_node_t * x)
{
	rb_node_t * y = l(x);
	p(y) = p(x);
	l(x) = r(y);
	if(r(y) != NIL_NODE){
		p(r(y)) = x;
	}
	if(p(x) == NIL_NODE){
		root = y;
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

static void rb_insert_fixup(rb_node_t * z)
{
	rb_node_t * y;
	while(c(p(z)) == RBT_RED){
		printf("current z_value is -- %d\n",i(z));
		/* p(z) not null and color is red,
		 * so p(z) is not root --> p(p(z)) exists */
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
				left_rotate(z);
			/*------------- case 3 --------------*/
			}else{
				/* z is left child of its parent */
				c(p(z)) = RBT_BLACK;
				c(p(p(z))) = RBT_RED;
				right_rotate(p(p(z)));
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
				right_rotate(z);
			}else{
				c(p(z)) = RBT_BLACK;
				c(p(p(z))) = RBT_RED;
				left_rotate(p(p(z)));
			}
		}
	}
	c(root) = RBT_BLACK;
	return;
}

void rb_insert(rb_node_t * z)
{
	int di;
	rb_node_t * node = raw_search_node(i(z),&di);
	printf("rb_insert : search_node -- di : %d\n",di);
	p(z) = node;
	if(node == NIL_NODE){
		/* empty rb_tree */
		printf("rb_insert : first node will be inserted!\n");
		root = z;
	}else if(di == 0){
		/* node with key==z->i already exists */
		printf("key already exists!\n");
		return;
	}else{
		if(di == -1){
			l(node) = z;
		}else if(di == 1){
			r(node) = z;
		}
	}
	l(z) = NIL_NODE;
	r(z) = NIL_NODE;
	c(z) = RBT_RED;
	rb_insert_fixup(z);
}

static void rb_delete_fixup(rb_node_t * z)
{
	rb_node_t * x = z,*w;
	while(x != root && c(x) == RBT_BLACK){
		if(x == l(p(x))){
			w = r(p(x));
			/*-------- case 1 --------*/
			if(c(w) == RBT_RED){
				/* brother of x is red,
				 * so c(p(x)) is black,
				 * repaint and rotate to give x a brother whose color is black */
				c(w) = RBT_BLACK;
				c(p(x)) = RBT_RED;
				left_rotate(p(x));
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
				right_rotate(w);
				w = r(p(x));
			/*-------- case 4 --------*/
			}else{
				/* right child of w is red,
				 * w is black. */
				c(w) = c(p(x));
				c(p(x)) = RBT_BLACK;
				c(r(w)) = RBT_BLACK;
				left_rotate(p(x));
				x = root;
			}
		}else if(x == r(p(x))){
			w = l(p(x));
			if(c(w) == RBT_RED){
				c(w) = RBT_BLACK;
				c(p(x)) = RBT_RED;
				right_rotate(p(x));
				w = l(p(x));
			}else if(c(l(w)) == RBT_BLACK && c(r(w)) == RBT_BLACK){
				c(w) = RBT_RED;
				x = p(x);
			}else if(c(l(w)) == RBT_BLACK){
				/* right child of w is red */
				c(w) = RBT_RED;
				c(r(w)) = RBT_BLACK;
				left_rotate(w);
				w = l(p(x));
			}else{
				/* left child of w is red */
				c(w) = c(p(x));
				c(l(w)) = RBT_BLACK;
				c(p(x)) = RBT_BLACK;
				right_rotate(p(x));
				x = root;
			}
		}
	}
	c(x) = RBT_BLACK;
}

rb_node_t * rb_delete(int key)
{
	int di;
	rb_node_t *x,*y;
	rb_node_t * z = raw_search_node(key,&di);
	if(z == NIL_NODE || di != 0){
		fprintf(stderr,"NO NODE WITH KEY %d\n",key);
		return NIL_NODE;
	}
	/* z != NIL_NODE && di == 0 */
	/* y is the node to be deleted */
	if(l(z) == NIL_NODE || r(z) == NIL_NODE){
		y = z;
	}else{
		y = rbt_suc(z);
	}
	if(l(y) != NIL_NODE){
		x = l(y);
	}else{
		x = r(y);
	}
	p(x) = p(y);
	if(p(y) == NIL_NODE){
		root = x;
	}else{
		if(y == l(p(y))){
			l(p(y)) = x;
		}else if(y == r(p(y))){
			r(p(y)) = x;
		}
	}
	if(y != z){
		i(z) = i(y);
	}
	if(c(y) == RBT_BLACK){
		rb_delete_fixup(x);
	}
	return y;
}

