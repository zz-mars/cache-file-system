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
