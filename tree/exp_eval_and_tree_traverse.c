#include "binary_tree.h"
#include "stack.h"

#define OPERATORS_N	7

static char operators[OPERATORS_N] = {'+','-','*','/','(',')','#'};

#define IS_OPERATOR(c) ({	\
		char _c = c;	\
		int i = 0;	\
		for(i=0;i<OPERATORS_N;i++) {	\
			if (operators[i] == _c) {	\
				break;	\
			}	\
		}	\
		(i < OPERATORS_N ? i : -1);	\
})

#define IS_EXPR(c)	({	\
		char _c = c;	\
		((_c <= 'z' && _c >= 'a') ||	\
		 (_c <='Z' && _c >= 'A')	||	\
		 (_c <= '9' && _c >= '0') );	\
})

static int privilege_table[OPERATORS_N][OPERATORS_N] = {	\
	1,1,-1,-1,-1,1,1,	\
	1,1,-1,-1,-1,1,1,	\
	1,1,1,1,-1,1,1,	\
	1,1,1,1,-1,1,1,	\
	-1,-1,-1,-1,-1,0,2,	\
	1,1,1,1,2,1,1,	\
	-1,-1,-1,-1,-1,2,0	\
};

tnode * expr_to_tree(char * expr)
{
	stack *opStack = newStack(STK_SZ);
	assert(opStack != NULL);
	stack *expStack = newStack(STK_SZ);
	assert(expStack != NULL);

	int i = 0;
	int sl = strlen(expr);
	while(i < sl) {
		int j = IS_OPERATOR(expr[i]);
		if(j >= 0) {

			/* operator */
			tnode *tn = newTnode(expr[i]);
			assert(tn != NULL);

			if(opStack->empty(opStack)) {
				/* push to operator stack when it is null */
				opStack->push(opStack,tn);
				i++;
				continue;
			}

			tnode *top = opStack->top(opStack);
			assert(top != NULL);

			int k = IS_OPERATOR(c(top));
			assert(k>=0 && k <= OPERATORS_N);

			switch(privilege_table[k][j]) {
				case 1:
					{
						/* new operator is less privileged than the former one */
						tnode *tr = expStack->pop(expStack);
						assert(tr != NULL);

						tnode *tl = expStack->pop(expStack);
						assert(tl != NULL);

						tnode *tp = opStack->pop(opStack);
						assert(tp != NULL);

						/* make a tree */
						addNewTnode(tl,tp,TNODE_LC);
						addNewTnode(tr,tp,TNODE_RC);
						expStack->push(expStack,tp);
					}
					break;
				case -1:
					opStack->push(opStack,tn); 
					i++;
					break;
				case 0:
					{
						tnode *t = opStack->pop(opStack);
						free(t);
						free(tn);
						i++;
					}
					break;
				default:
					fprintf(stderr,"invalid expression\n");
					return NULL;
			}
		}else if(IS_EXPR(expr[i])) {
			tnode *tn = newTnode(expr[i]);
			assert(tn != NULL);
			expStack->push(expStack,tn);
			i++;
		}else {
			fprintf(stderr,"invalid expression\n");
			return NULL;
		}
	}

	tnode *root = expStack->pop(expStack);

	/* all stack should be empty */
	assert(expStack->empty(expStack) && opStack->empty(opStack));

	delStack(opStack);
	delStack(expStack);

	return root;
}

int main()
{
	char *s = "#a+b*(c-d)-e/f#";

	tnode *root = expr_to_tree(s);

	printf("inOrderTraverse :\n");
	inOrderTraverse(root);
	printf("\n");

	printf("inOrderTraverseNoRecur :\n");
	inOrderTraverseNoRecur(root);

	tnode head;
	inOrderThreading(root,&head);
	printf("inOrderTraverseThread :\n");
	inOrderTraverseThread(&head);

	printf("preOrderTraverse :\n");
	preOrderTraverse(root);
	printf("\n");

	printf("preOrderTraverseNoRecur :\n");
	preOrderTraverseNoRecur(root);


	printf("postOrderTraverse :\n");
	postOrderTraverse(root);
	printf("\n");

	printf("postOrderTraverseNoRecur :\n");
	postOrderTraverseNoRecur(root);

	printf("postOrderTraverseNoRecur_ :\n");
	postOrderTraverseNoRecur_(root);

	return 0;
}

