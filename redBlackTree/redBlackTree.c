#include "redBlackTree.h"
#include "queue.h"

#define p(z)	(z)->p
#define l(z)	(z)->l
#define r(z)	(z)->r
#define c(z)	(z)->c
#define i(z)	(z)->i
#define d(z)	(z)->data

#ifdef LEVEL_PRINT

#define last_in_this_level(z)	(z)->last_in_this_level

#endif

static rb_node_t nil_node;

rb_node_t *NIL_NODE = NULL;

void rbt_init(redBlackTree_t *T)
{
	T->root = NIL_NODE = &nil_node;
	c(NIL_NODE) = RBT_BLACK;
	T->nodes_nr = 0;
}

static inline rb_node_t * new_rb_node(int key)
{
	rb_node_t * new_node = (rb_node_t *)malloc(sizeof(rb_node_t));
	if(new_node == NULL){
		perror("new rb_node_t");
		return NIL_NODE;
	}

	i(new_node) = key;
	l(new_node) = NIL_NODE;
	r(new_node) = NIL_NODE;
	p(new_node) = NIL_NODE;
	c(new_node) = RBT_RED;

#ifdef LEVEL_PRINT
	last_in_this_level(new_node) = 0;
#endif

	return new_node;
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

static int rbt_find_max_depth(rb_node_t *node)
{
	if(node == NIL_NODE) {
		return 0;
	}

	int ld = rbt_find_max_depth(l(node));
	int rd = rbt_find_max_depth(r(node));

	return (ld>rd?(ld+1):(rd+1));
}

int rbt_max_depth(redBlackTree_t *T)
{
	return rbt_find_max_depth(T->root);
}

#ifdef LEVEL_PRINT

static inline void print_space(int n)
{
	if(n <= 0) {
		return;
	}

	if(n > 127) {
		fprintf(stderr,"too much space!\n");
		return;
	}
	char space_buf[128];
	int i;
	memset(space_buf,' ',n);
	space_buf[n] = '\0';
	printf("%s",space_buf);
}

#define bin_power(n)	({	\
		int _n = n;	\
		int s = 1;	\
		int i;	\
		for(i=0;i<_n;i++) {	\
			s *= 2;	\
		}	\
		s;	\
	})

static inline void init_indent_and_spaces(int dep,int *indent,int *spaces)
{
	*spaces = bin_power(dep);
	*indent = (*spaces)/2 - 1;
//	printf("spaces %d indent %d\n",*spaces,*indent);
}

void rbt_level_traverse(redBlackTree_t *T)
{
	rb_node_t *node = T->root;
	if(node == NIL_NODE) {
		printf("NULL redBlackTree!\n");
		return;
	}

	int max_dep = rbt_max_depth(T);

//	printf("max depth %d\n",max_dep);

	last_in_this_level(node) = 1;
	queue_t *ltq = init_queue(INIT_QUEUE_SZ);
	if(!ltq) {
		return;
	}

	if(en_queue(ltq,node)) {
		goto destroy_q_and_ret;
	}

	int dep = max_dep;
	int indent,spaces;
	init_indent_and_spaces(dep--,&indent,&spaces);

	print_space(indent);
	while(!queue_empty(ltq)) {
		de_queue(ltq,(void**)&node);
		if(node == NIL_NODE) {
			print_space(spaces);
			continue;
		}

		printf("%d%c",i(node),c(node));
		print_space(spaces);

		last_in_this_level(l(node)) = last_in_this_level(r(node)) = 0;
		if(last_in_this_level(node)) {
			printf("\n\n");
			init_indent_and_spaces(dep--,&indent,&spaces);
			print_space(indent);

			if(l(node) == NIL_NODE && r(node) == NIL_NODE) {
				rb_node_t *n;
				int i = 0;
				while(queue_rear(ltq,i++,(void**)&n) == 0) {
					if(n == NIL_NODE) {
						continue;
					}
					last_in_this_level(n) = 1;
					break;
				}
				continue;
			}

			if(r(node) != NIL_NODE) {
				last_in_this_level(r(node)) = 1;
			}else if(l(node) != NIL_NODE) {
				last_in_this_level(l(node)) = 1;
			}
		}

		/* en_queue anyway */
		if(en_queue(ltq,l(node))) {
			break;
		}

		if(en_queue(ltq,r(node))) {
			break;
		}
	}

	printf("\n");

destroy_q_and_ret:
	destory_queue(ltq);
}

#endif

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
 *	We paint p(z) to black and p(p(z)) to red,and do the right rotateo on p(p(z)),
 *	after then the additional red color goes to z's uncle's side and
 *	the black-height property will stay unchanged.
 *
 * PS:ROTATE WILL NOT CHANGE THE BLACK-HEIGHT PROPERTY.
 *
 * FINALLY,WE SHOULD SET THE ROOT TO BLACK TO MAKE SURE WHEN THE FIRST NODE IS INSERTED,
 * IT WILL BE TRE ROOT AND BE BLACK.
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

/* We need to fix-up the redBlackTree only if we have deleted the 
 * node whose color is black.
 * If c(to_del) is red :
 *	1) Deleting a red node simply has no effect on the black-height property.
 *	2) No red nodes will be made adjacent when deleting a red node.
 *	3) This red node will not be the root when it is red,so the root stay black. 
 *
 * When deleting a black node y,a new node x is to replace y's place,
 * and y's black color will be pushed to x.So here x may get doubly black color 
 * or red-black color according to its original color.
 * This violates the principle that every node can be only red or black.
 * So the goal of rbt_delete_fixup is to absorb the additional black color on 
 * the node. To make this,we consider the following.
 *
 * Still we only consider the case when x is left child of its parent,and there's 
 * a symmetric one when x is right child of its parent.
 *
 * For the case of x == l(p(x)),there are some subcases.
 * First of all,we should be aware that x is black and it must have a brother to make
 * the same black-height on both side of its parent.Assume w is x's brother.
 *
 * 1. When w is red,x and w 's parent must be black and w must have two children whose 
 * color are black.We do the following :
 *		c(p(x)) = RBT_RED;
 *		c(w) = RBT_BLACK;
 *		left_rotate(T,p(x));
 *		w = r(p(x));
 * First we repaint p(x) to red and w to black,then do the left rotation,w is re-assigned
 * to be x's brother.But now,w is black,which means x now has a black brother.Now we go
 * to the following cases.
 *
 * 2. When w is black,we get three subcases according to the color of w's children.
 *
 *	2.1 Both of w's left and right child are black
 *	As x and w are black too,we can simply push	the additional black color upward to x's parent,
 *	so here x become singly black and w become red.It will be okay for w to be red because both
 *	of its children are black.We then reassign x to be p(x) which now has color of doubly black 
 *	or red-black according to its original color.A new loop will be started with new x.
 *
 *	Mention that although now x has an additional black color,this is not reflected in the c(x) property,
 *	which means c(x) still represents its original color.JUST REMEMBER X IS THE NODE WHO HAS AN ADDITIONAL
 *	BLACK COLOR BESIDES ITS ORIGINAL COLOR.
 *	The loop will terminate if c(x) == RBT_RED,then we simply repaint x to be black,which means,when x
 *	is red and it has additional black color,the fix-up will end simply by pushing the black color into x.
 *	If the new x is originally black too and now it has doubly black color,the loop will continue to absorb
 *	the additional black color.
 *
 *	2.2 w's left child is red and right child is black
 *	We do the following:
 *		c(l(w)) = RBT_BLACK;
 *		c(w) = RBT_RED;
 *		right_rotate(T,w);
 *		w = r(p(x));
 *	By doing these operations,w is still x's black brother.But now w has a red right child,which is the next case.
 *
 *	2.3 w's right child is red
 *	Now we get a view that both w and x is black,w's right child is red.We do the following:
 *		c(w) = c(p(w));
 *		c(p(x)) = c(r(w)) = RBT_BLACK;
 *		left_rotate(T,p(x));
 *		x = T->root;
 *	Considering the view,x and w are black,w's right child is red,which can be used to absorb w's black color.
 *	There are some color transfers in the operations above,which are essentially as following:
 *
 *	1) w's black color				----------->	r(w) ( which is originally red )
 *
 *	2) p(x)'s color					----------->	w ( whose color has been absorbed by r(w) )
 *
 *	Now w has the color of p(x),and after a left-rotation,it will replace p(x).
 *	p(x) now is free to aborb the additional black color on x.
 *
 *	3) x's additional black color	----------->	p(x) ( whose color has been absorbed by w )
 *	
 *	After the color transfer,we do the left rotation and the additional black color is aborbed finally.
 *	And then the loop should end.We do this by assigning x with T->root,which will be tested before the next
 *	loop starts,then the loop ends.
 *
 *	---------------------------------- post script ----------------------------------
 *
 *	Q1: When the fix-up operaions is needed?
 *	A1: When x has doubly black color and it is not the root,a fix-up operation is needed.
 *		If x is originally red,the additional black color will simply be absorbed.
 *		Or if x is root,although it has doubly black color,it will just be okay to keep only one black color
 *		just because it is the root,anything it does has the same effect on all the nodes.
 *
 *	Q2: What essentially do the fix-up operations do?
 *	A2: To absorb the additional black color.To make this,we should find some free node to absorb it.
 *		Candidates are x's parent node,who is used to absorb x's additional black color in two situations :
 *
 *		1) when x's brother w is black and both w's children are black,black color goes upwards to p(x),
 *		and then x is still black and w becomes red.
 *		Loop continue with x = p(x).
 *
 *		2) x's brother w is black and r(w) is red.As disscussed,some color transfers help to absorb the additional
 *		black color.
 *
 *	*/
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

