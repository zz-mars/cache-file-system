#include "binary_tree.h"

int main()
{
	char *inOrder = "a+b*c-d/e";
	char *postOrder = "abcd-*e/+";
	char *preOrder = "+a/*b-cde";

	int inlen = strlen(inOrder);
	int postlen = strlen(postOrder);

	if(inlen != postlen) {
		fprintf(stderr,"invalid input!\n");
		return 1;
	}

	printf("inOrderTraverse : %s\npostOrderTraverse : %s\npreOrderTraverse : %s\n",
			inOrder,postOrder,preOrder);

	tnode *root;
	if(build_tree_InPost(inOrder,postOrder,inlen,&root)) {
		fprintf(stderr,"build tree from inOrder and postOrder fail!\n");
		return 1;
	}

	printf("build tree from in_post:\n");
	inOrderTraverse(root);
	printf("\n");
	postOrderTraverse(root);
	printf("\n");
	preOrderTraverse(root);
	printf("\n");

	delTree(root);

	printf("build tree from in_pre\n");
	if(build_tree_InPre(inOrder,preOrder,inlen,&root)) {
		fprintf(stderr,"build tree from inOrder and preOrder fail!\n");
		return 1;
	}

	inOrderTraverse(root);
	printf("\n");
	postOrderTraverse(root);
	printf("\n");
	preOrderTraverse(root);
	printf("\n");

	delTree(root);

	return 0;
}

