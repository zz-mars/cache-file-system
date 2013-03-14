#include"glob.h"
#define RBT_RED			'R'
#define RBT_BLACK		'B'
#define p(z)			z->p
#define l(z)			z->l
#define r(z)			z->r
#define c(z)			z->c
#define i(z)			z->i
#define RED_STR			"red"
#define BLACK_STR		"black"
#define LEFT_CHILD		01
#define PARENT			02
#define RIGHT_CHILD		03
#define H_LEFT_CHILD	"LEFT_CHILD : "
#define H_PARENT		"PARENT : "
#define H_RIGHT_CHILD	"RIGHT_CHILD : "
#define ELEMENTS_NUM	13
typedef struct RBT{
	u32 i;
	u8 c;
	struct RBT * l;
	struct RBT * r;
	struct RBT * p;
}rbt;
static u32 a[ELEMENTS_NUM] = {2,399,387,219,266,382,381,278,363,100,250,240,245};
static rbt rbt_nil_node;
static rbt * NIL_NODE;
static rbt * root;
static void init_rbt()
{
	rbt_nil_node.c = RBT_BLACK;
	NIL_NODE = &rbt_nil_node;
	root = NIL_NODE;
}
static rbt * get_node_by_key(u32 key,u32 * di)
{
	rbt * node = root;
	if(node == NIL_NODE){
		*di = 0;
		return NIL_NODE;
	}
	while(1){
		if(key < i(node)){
			if(l(node) == NIL_NODE){
				*di = -1;
				break;
			}else{
				node = l(node);
			}
		}else if(key > i(node)){
			if(r(node) == NIL_NODE){
				*di = 1;
				break;
			}else{
				node = r(node);
			}
		}else{
			*di = 0;
			break;
		}
	}
	return node;
}
static void left_rotate(rbt * x)
{
	rbt * y = r(x);
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
	return;
}
static void right_rotate(rbt * x)
{
	rbt * y = l(x);
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
	return;
}
static rbt * rb_max(rbt * z)
{
	while(r(z) != NIL_NODE){z = r(z);}
	return z;
}
static rbt * rb_min(rbt * z)
{
	while(l(z) != NIL_NODE){z = l(z);}
	return z;
}
static rbt * rb_suc(rbt * z)
{
	rbt * x;
	if(z == NIL_NODE){
		fprintf(stderr,"NO SUC_NODE FOR NIL_NODE\n");
		return z;
	}
	if(r(z) != NIL_NODE){
		return rb_min(r(z));
	}
	x = z;
	while( p(x) != NIL_NODE && x == r(p(x))){
		x = p(x);
	}
	return p(x);
}
static rbt * rb_pre(rbt * z)
{
	rbt * x;
	if(z == NIL_NODE){
		fprintf(stderr,"NO PRE_NODE FOR NIL_NODE\n");
		return z;
	}
	if(l(z) != NIL_NODE){
		return rb_max(l(z));
	}
	x = z;
	while(p(x) != NIL_NODE && x == l(p(x))){
		x = p(x);
	}
	return p(x);
}
static rbt * new_rbt_node(u32 key)
{
	rbt * new_node = (rbt *)malloc(sizeof(rbt));
	if(new_node == NULL){
		return new_node;
	}
	i(new_node) = key;
	l(new_node) = NIL_NODE;
	r(new_node) = NIL_NODE;
	p(new_node) = NIL_NODE;
	c(new_node) = RBT_RED;
	return new_node;
}
static void print_rbt_node(rbt * node,u8 i)
{
	u8 * lpr;
	if(node == NIL_NODE){
		fprintf(stderr,"CANNOT PRINT A NULL NODE!\n");
		return;
	}
	switch(i){
		case LEFT_CHILD:
			lpr = H_LEFT_CHILD;
			break;
		case PARENT:
			lpr = H_PARENT;
			break;
		case RIGHT_CHILD:
			lpr = H_RIGHT_CHILD;
			break;
		default:
			fprintf(stderr,"UNRECOGNIZED I\n");
			return;
	}
	printf("%s%3d -- %c\n",lpr,i(node),c(node));
	return;
}
static void print_rbt(rbt * r)
{
	rbt * n = r;
	u8 * s;
	if(n == NIL_NODE){
		return;
	}
	print_rbt_node(n,PARENT);
	if(l(n) != NIL_NODE){
		print_rbt_node(l(n),LEFT_CHILD);
	}
	if(r(n) != NIL_NODE){
		print_rbt_node(r(n),RIGHT_CHILD);
	}
	printf("------------------------------------\n");
	print_rbt(l(n));
	print_rbt(r(n));
	return;
}
static void rb_insert_fixup(rbt * z)
{
	rbt * y;
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
static void rb_insert(rbt * z)
{
	u32 di;
	rbt * node = get_node_by_key(i(z),&di);
	printf("rb_insert : get_node_by_key -- di : %d\n",di);
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
	return;
}
static void rb_delete_fixup(rbt * z)
{
	rbt * x = z,*w;
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
	return;
}
static rbt * rb_delete(u32 key)
{
	u32 di;
	rbt *x,*y;
	rbt * z = get_node_by_key(key,&di);
	if(z == NIL_NODE || di != 0){
		fprintf(stderr,"NO NODE WITH KEY %d\n",key);
		return NIL_NODE;
	}
	/* z != NIL_NODE && di == 0 */
	/* y is the node to be deleted */
	if(l(z) == NIL_NODE || r(z) == NIL_NODE){
		y = z;
	}else{
		y = rb_suc(z);
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
int main()
{
	u32 i,di;
	rbt * node,*tmp;
	init_rbt();
	/*-------------- insert some nodes ------------------*/
	for(i = 0;i < ELEMENTS_NUM;i++){
		printf("now insert node -- %d\n",a[i]);
		if((node = new_rbt_node(a[i])) == NULL){
			fprintf(stderr,"NEW_RBT_NODE FAIL : KEY -- %d\n",a[i]);
			break;
		}
		rb_insert(node);
		print_rbt(root);
		printf("---------------------------------------------------------------------------------------------------------\n");
	}
//	/*-------------- test rbt_max --  rbt_min  --  rbt_suc -- rbt_pre component ---------------*/
//	node = rb_max(root);
//	if(node != NIL_NODE){
//		printf("max of rbt -- %d\n",i(node));
//	}
//	node = rb_min(root);
//	if(node != NIL_NODE){
//		printf("min of rbt -- %d\n",i(node));
//	}
//	for(i = 0;i<ELEMENTS_NUM;i++){
//		node = get_node_by_key(a[i],&di);
//		if(node == NIL_NODE || di != 0){
//			fprintf(stderr,"get_node_by_key fail : KEY -- %d\n",a[i]);
//			break;
//		}
//		tmp = rb_suc(node);
//		if(tmp == NIL_NODE){
//			printf("NO SUC_NODE FOR -- %d\n",i(node));
//		}else{
//			printf("SUC_NODE FOR %d is -- %d\n",i(node),i(tmp));
//		}	
//		tmp = rb_pre(node);
//		if(tmp == NIL_NODE){
//			printf("NO PRE_NODE FOR -- %d\n",i(node));
//		}else{
//			printf("PRE_NODE FOR %d is -- %d\n",i(node),i(tmp));
//		}
//	}
//	/*-------------- test rotate component ----------------------*/
//	printf("--------------------- 1st rotate ----------------\n");
//	node = root;
//	left_rotate(node);
//	print_rbt(root);
//	printf("--------------------- rotate back ----------------\n");
//	node = root;
//	right_rotate(node);
//	print_rbt(root);
//	printf("--------------------- rotate 266 ----------------\n");
//	node = get_node_by_key(266,&di);
//	left_rotate(node);
//	print_rbt(root);
//	printf("--------------------- rotate 381 ----------------\n");
//	node = get_node_by_key(381,&di);
//	right_rotate(node);
//	print_rbt(root);

	/*---------------- test delete node component ----------------*/
	printf("--------------- delete 219 ------------\n");
	node = rb_delete(219);
	print_rbt(root);
	return 0;
}
