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
	}

	printRedBlackTree(&T);

	rbt_delete(&T,381);

	printRedBlackTree(&T);

	rbt_destory(&T);

	return 0;
}
