#ifndef _SORT_H
#define _SORT_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>

static inline void randGenerator(int a[],int len,int limit)
{
	int i;
	for(i = 0;i < len;i++){
		srand(time(NULL) + i);
		a[i] = rand()%limit;
	}
}

static inline void printIntarray(int a[],int len)
{
	if(len < 0) {
		fprintf(stderr,"invalid array length!\n");
		return;
	}

	int i;
	for(i=0;i<len;i++) {
		printf("%d ",a[i]);
	}
	printf("\n");
}

static inline void swapElem(int a[],int i,int j)
{
	if(i == j) {
		return;
	}

	int tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
}

static inline int max_of_array(int a[],int len)
{
	int i;
	int max = a[0];
	for(i=1;i<len;i++) {
		if(a[i] > max) {
			max = a[i];
		}
	}
	return max;
}

static inline int rpower(int m,int n)
{
	if(n == 0) {
		return 1;
	}

	int i=m,j;
	for(j=0;j<n;j++) {
		i *= m;
	}
	return i;
}

static inline int low_bit_of_num(int a,int i)
{
	return ((a/rpower(10,i)) % 10);
}

typedef void (*sortFunc)(int a[],int len);

void insertionSort(int a[],int len);

void selectSort(int a[],int len);

void shellSort(int a[],int len);

void heapSort(int a[],int len);

void quickSort(int a[],int len);

void mergeSort(int a[],int len);

void bubbleSort(int a[],int len);

void radixSort(int a[],int len);

void bucketSort(int a[],int len);

void countingSort(int a[],int len);

#endif
