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

int find_kth_largest(int a[], int n, int k) {
	if(k >= n) {
		return -1;
	}
	int pivot_idx = quick_partition(a, 0, n);
	if(pivot_idx == -1) {
		// fatal
		return -1;
	}
	int reverse_idx = n - pivot_idx - 1;
	if(reverse_idx == k) {
		return a[pivot_idx];
	}
	return reverse_idx>k?find_kth_largest(a+pivot_idx+1, reverse_idx, k):find_kth_largest(a, pivot_idx, k-reverse_idx-1);
}

int main(int argc, char * argv[]) {
	int a[ARRAY_LEN];
	randGenerator(a, ARRAY_LEN, 500);
	printIntarray(a, ARRAY_LEN);
	int x = find_kth_largest(a, ARRAY_LEN, 3);
	printf("third -> %d\n", x);
	quick_sort(a, 0, ARRAY_LEN);
//	quick_partition(a, 0, ARRAY_LEN);
	printIntarray(a, ARRAY_LEN);
	return 0;
}
