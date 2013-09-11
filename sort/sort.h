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

static inline void max_min_of_array(int a[],int len,int *max,int *min)
{
	if((!max && !min) || len <= 0) {
		fprintf(stderr,"invalid arg!\n");
		return;
	}

	int ma,mi,i;
	if(len == 1) {
		ma = mi = a[0];
		goto ret;
	}

	if(len % 2 == 0) {
		if(a[0] < a[1]) {
			ma = a[1];
			mi = a[0];
		}else {
			ma = a[0];
			mi = a[1];
		}
		i = 2;
	}else {
		ma = mi = a[0];
		i = 1;
	}

	while(i < len) {
		int _ma,_mi;
		if(a[i] < a[i+1]) {
			_ma = a[i+1];
			_mi = a[i];
		}else {
			_ma = a[i];
			_mi = a[i+1];
		}
		if(_ma > ma) {
			ma = _ma;
		}
		if(_mi < mi) {
			mi = _mi;
		}
		i += 2;
	}

ret:
	if(max) {
		*max = ma;
	}
	if(min) {
		*min = mi;
	}
}

static inline int max_of_array(int a[],int len)
{
	int max;
	max_min_of_array(a,len,&max,NULL);
	return max;
}

static inline int min_of_array(int a[],int len)
{
	int min;
	max_min_of_array(a,len,NULL,&min);
	return min;
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

static inline int quickPartition(int a[],int s,int e)
{
	int pivot = a[e];
	int i;
	int last_elem_lt_pivot = s - 1;
	for(i=s;i<e;i++) {
		if(a[i] < pivot) {
			swapElem(a,i,++last_elem_lt_pivot);
		}
	}
	swapElem(a,e,++last_elem_lt_pivot);
	return last_elem_lt_pivot;
}

static inline int len_of_num(int a)
{
	int l = 0;
	int i = a;
	do {
		l++;
		i /= 10;
	} while(i != 0);
	return l;
}

static inline int biggest_len(int a[],int len)
{
	int b = 0;
	int i;
	for(i=0;i<len;i++) {
		int l = len_of_num(a[i]);
		if(l > b) {
			b = l;
		}
	}
	return b;
}

static inline int selectMinKey(int a[],int m,int n)
{
	int i;
	int minKey = m;
	for(i=m+1;i<=n;i++) {
		if(a[minKey] > a[i]){
			minKey = i;
		}
	}
	return minKey;
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
