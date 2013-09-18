#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define ELEM_N	10

#define visit(i,j)	do {	\
	int _i = i;	\
	int _j = j;	\
	printf("(%d,%d) ",_i,_j);	\
}while(0)

int main()
{
	int i,j;
	i = j = 0;

	while(i<ELEM_N && j<ELEM_N) {
		if(i > 0) {
			while(i>=0) {
				visit(i--,j++);
			}
			i = 0;
		}else if(j > 0) {
			while(j >= 0) {
				visit(i++,j--);
			}
			j = 0;
		}else if(i == 0 && j == 0) {
			visit(i++,j);
		}
	}

	printf("\n i = %d j = %d\n",i,j);

	if(i == ELEM_N) {
		i = ELEM_N - 1;
		j = 1;
	}else if(j == ELEM_N) {
		j = ELEM_N - 1;
		i = 1;
	}

	while(1) {
		if(i < ELEM_N - 1) {
			while(i <= ELEM_N - 1) {
				visit(i++,j--);
			}
			j += 2;
			i = ELEM_N - 1;
		}else if(j < ELEM_N - 1) {
			while(j <= ELEM_N - 1) {
				visit(i--,j++);
			}
			i += 2;
			j = ELEM_N - 1;
		}else if(i == ELEM_N - 1 && j == ELEM_N - 1) {
			visit(i,j);
			break;
		}
	}

	printf("\n i = %d j = %d\n",i,j);

	return 0;
}
