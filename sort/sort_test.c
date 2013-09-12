#include "sort.h"

#define ELEM_N	30

int main()
{
	int a[ELEM_N];
	randGenerator(a,ELEM_N,1000);

	printIntarray(a,ELEM_N);
//	insertionSort(a,ELEM_N);
//	selectSort(a,ELEM_N);
//	shellSort(a,ELEM_N);
//	bubbleSort(a,ELEM_N);
	quickSort(a,ELEM_N);
//	heapSort(a,ELEM_N);
//	radixSort(a,ELEM_N);
//	countingSort(a,ELEM_N);
//	mergeSort(a,ELEM_N);
	printIntarray(a,ELEM_N);

	return 0;
}

