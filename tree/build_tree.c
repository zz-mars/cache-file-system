#include "binary_tree.h"

static inline int idx_of_char(char *str,int len,char p)
{
	int i = 0;
	for(i=0;i<len;i++) {
		if(str[i] == p) {
			break;
		}
	}
	return (i == len ?-1:i);
}

enum {
	BUILD_FROM_IN_POST = 0,
	BUILD_FROM_IN_PRE
};

static int raw_build_tree(char *inOrderStr,char *ppStr,int len,tnode **root,char pp)
{
	if(pp != BUILD_FROM_IN_PRE && pp != BUILD_FROM_IN_POST) {
		fprintf(stderr,"invalid arg!\n");
		return 1;
	}

	if(!root) {
		fprintf(stderr,"invalid arg!\n");
		return 1;
	}

	*root = NULL;

	if(len == 0) {
		return 0;
	}

	char *r = pp==BUILD_FROM_IN_POST?ppStr+len-1:ppStr;

	int i = idx_of_char(inOrderStr,len,*r);
	if(i == -1) {
		fprintf(stderr,"invalid arg!\n");
		return 1;
	}

	tnode *lchild,*rchild;
	char *ls = pp==BUILD_FROM_IN_POST?ppStr:ppStr+1;
	char *rs = pp==BUILD_FROM_IN_POST?ppStr+i:ppStr+i+1;

	if(raw_build_tree(inOrderStr,ls,i,&lchild,pp)) {
		return 1;
	}

	if(raw_build_tree(inOrderStr+i+1,rs,len-i-1,&rchild,pp)) {
		return 1;
	}

	*root = newTnode(*r);

	if(!*root) {
		perror("newTnode");

		if(lchild) {
			free(lchild);
		}

		if(rchild) {
			free(rchild);
		}

		return 1;
	}

	l(*root) = lchild;
	r(*root) = rchild;

	if(lchild) {
		p(lchild) = *root;
	}

	if(rchild) {
		p(rchild) = *root;
	}

	return 0;
}

int build_tree_InPre(char *inOrderStr,char *preOrderStr,int len,tnode **root)
{
	return raw_build_tree(inOrderStr,preOrderStr,len,root,BUILD_FROM_IN_PRE);
}

int build_tree_InPost(char *inOrderStr,char *postOrderStr,int len,tnode **root)
{
	return raw_build_tree(inOrderStr,postOrderStr,len,root,BUILD_FROM_IN_POST);
}

