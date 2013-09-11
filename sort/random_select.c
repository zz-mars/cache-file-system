#include "sort.h"

int random_select(int a[],int s,int e,int i)
{
	if(i > (e-s) || i < 0) {
		fprintf(stderr,"invalid arg!\n");
		return -1;
	}

	int pivot_idx = quickPartition(a,s,e);

	int selected_idx = s + i;
	if(pivot_idx == selected_idx) {
		return a[pivot_idx];
	}

	if(pivot_idx > selected_idx) {
		return random_select(a,s,pivot_idx-1,i);
	}

	return random_select(a,pivot_idx+1,e,s+i-pivot_idx-1);
}

#define ELEM_N	12

int main()
{
	int a[ELEM_N];
	randGenerator(a,ELEM_N,100);
	printIntarray(a,ELEM_N);

	int s = 4;
	int i = random_select(a,0,ELEM_N-1,s);

	printf("%dth elem : %d\n",s,i);
	
	quickSort(a,ELEM_N);
	printIntarray(a,ELEM_N);
	return 0;
}
