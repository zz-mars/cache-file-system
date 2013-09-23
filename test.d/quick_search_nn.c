#include <stdio.h>
#include <assert.h>

#define ELEM_N	5

static int a[ELEM_N][ELEM_N] = {1,5,6,10,13,\
								2,7,19,22,27,\
								14,20,25,30,33,\
								18,26,31,37,40,\
								23,35,38,42,47};

static inline void print_nn(void)
{
	int i;
	for(i=0;i<ELEM_N;i++) {
		int j;
		for(j=0;j<ELEM_N;j++) {
			printf("%2d ",a[i][j]);
		}
		printf("\n");
	}
}

enum {
	FIND_X_IN_NN_EARG = 0,
	FIND_X_IN_NN_FOUND,
	FIND_X_IN_NN_NOT_FOUND
};

int find_x_in_nn(int a[][ELEM_N],int key,int *i,int *j)
{
	if(!i || !j) {
		return FIND_X_IN_NN_EARG;
	}

	int xLimit,yLimit;

	for(xLimit=ELEM_N-1;xLimit>=0;xLimit--) {
		if(a[xLimit][0] <= key) {
			break;
		}
	}

	if(xLimit == -1) {
		return FIND_X_IN_NN_NOT_FOUND;
	}


	for(yLimit=ELEM_N-1;yLimit>=0;yLimit--) {
		if(a[0][yLimit] <= key) {
			break;
		}
	}

	assert(yLimit >= 0);

	printf("find_x_in_nn -- xLimit %d yLimit %d\n",xLimit,yLimit);

	int y,x = xLimit;
	while(x>=0) {
		for(y=yLimit;y>=0;y--) {
			if(a[x][y] == key) {
				*i = x;
				*j = y;
				return FIND_X_IN_NN_FOUND;
			}
		}
		x--;
	}

	return FIND_X_IN_NN_NOT_FOUND;
}

int main(int argc,char *argv[])
{
	print_nn();

	int i,j,key;

	printf("input a number : ");
	scanf("%d",&key);
	printf("\n");

	while(1) {

		if(key == -1) {
			break;
		}

		int cs = find_x_in_nn(a,key,&i,&j);
		switch(cs) {
			case FIND_X_IN_NN_FOUND:
				printf("key %d -- a[%d,%d]\n",key,i,j);
				break;
			case FIND_X_IN_NN_NOT_FOUND:
				printf("key %d -- not found!\n",key);
				break;
		}

		printf("input a number : ");
		scanf("%d",&key);
		printf("\n");
	}

	return 0;
}

