#include "sort.h"

#define ARRAY_LEN	10

int quick_partition(int a[], int s, int e) {
	if (s >= e) {
		return -1;
	}
	int pivot = s;
	int swap_idx = s + 1;
	int idx;
	for(idx=s+1;idx<e;idx++) {
		if(a[idx] < a[pivot]) {
			swapElem(a, idx, swap_idx);
			swap_idx++;
		}
	}
	swap_idx--;
	swapElem(a, pivot, swap_idx);
	printf("swap_idx => %d\n", swap_idx);
	return swap_idx;
}

void quick_sort(int a[], int s, int e) {
	if( s >= e) {
		return;
	}
	int pivot = quick_partition(a, s, e);
	printf("pivot -> %d\n", pivot);
	if(pivot == -1) {
		return;
	}
	quick_sort(a, s, pivot);
	quick_sort(a, pivot+1, e);
}

int main(int argc, char * argv[]) {
	int a[ARRAY_LEN];
	randGenerator(a, ARRAY_LEN, 500);
	printIntarray(a, ARRAY_LEN);
	quick_sort(a, 0, ARRAY_LEN);
//	quick_partition(a, 0, ARRAY_LEN);
	printIntarray(a, ARRAY_LEN);
	return 0;
}
