#include "redBlackTree.h"

int findMaxDepth(rb_node_t *node)
{
	if(node == NIL_NODE) {
		return 0;
	}

	int ld = findMaxDepth(l(node));
	int rd = findMaxDepth(r(node));

	return (ld>rd?(ld+1):(rd+1));
}

int rbtMaxDepth(redBlackTree_t *T)
{
	return findMaxDepth(T->root);
}

int rbtPrint(redBlackTree_t *T)
{
}
