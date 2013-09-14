#include "redBlackTree.h"

#define ELEMENTS_NUM	13

static int a[ELEMENTS_NUM] = {2,399,387,219,266,382,381,278,363,100,250,240,245};

int main()
{
	int i,di;
	rb_node_t * node,*tmp;

	redBlackTree_t T;

	rbt_init(&T);

	/*-------------- insert some nodes ------------------*/
	for(i = 0;i < ELEMENTS_NUM;i++){
		printf("now insert node -- %d\n",a[i]);
		rbt_insert(&T,a[i]);
		printRedBlackTree(&T);
		printf("------------------------------------------------------------\n");
	}
//	/*-------------- test rb_node_t_max --  rb_node_t_min  --  rb_node_t_suc -- rb_node_t_pre component ---------------*/
//	node = rb_max(root);
//	if(node != NIL_NODE){
//		printf("max of rb_node_t -- %d\n",i(node));
//	}
//	node = rb_min(root);
//	if(node != NIL_NODE){
//		printf("min of rb_node_t -- %d\n",i(node));
//	}
//	for(i = 0;i<ELEMENTS_NUM;i++){
//		node = search_node(a[i],&di);
//		if(node == NIL_NODE || di != 0){
//			fprintf(stderr,"search_node fail : KEY -- %d\n",a[i]);
//			break;
//		}
//		tmp = rb_suc(node);
//		if(tmp == NIL_NODE){
//			printf("NO SUC_NODE FOR -- %d\n",i(node));
//		}else{
//			printf("SUC_NODE FOR %d is -- %d\n",i(node),i(tmp));
//		}	
//		tmp = rbt_pre(node);
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
//	print_rb_node_t(root);
//	printf("--------------------- rotate back ----------------\n");
//	node = root;
//	right_rotate(node);
//	print_rb_node_t(root);
//	printf("--------------------- rotate 266 ----------------\n");
//	node = search_node(266,&di);
//	left_rotate(node);
//	print_rb_node_t(root);
//	printf("--------------------- rotate 381 ----------------\n");
//	node = search_node(381,&di);
//	right_rotate(node);
//	print_rb_node_t(root);

	/*---------------- test delete node component ----------------*/
//	printf("--------------- delete 219 ------------\n");
//	node = rb_delete(219);

	printRedBlackTree(&T);
	rbt_destory(&T);

	return 0;
}
