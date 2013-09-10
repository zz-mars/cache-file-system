#include "sort.h"

void rawInsert(int a[],int len,int step)
{
	int i;
	for(i=step;i<len;i+=step) {
		int tmp = a[i];
		int j;
		for(j=i-step;j>=0;j-=step) {
			if(a[j] > tmp) {
				a[j+step] = a[j];
			}else{
				break;
			}
		}
		a[j+step] = tmp;
	}
}

void insertionSort(int a[],int len)
{
	rawInsert(a,len,1);
}

void shellSort(int a[],int len)
{
	int step[3] = {3,2,1};
	int i;
	for(i=2;i>=0;i--) {
		rawInsert(a,len,step[i]);
	}
}

inline int selectMin(int a[],int m,int n)
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

void selectSort(int a[],int len)
{
	int i;
	for(i=0;i<len;i++) {
		int min = selectMin(a,i,len-1);
		swapElem(a,i,min);
	}
}

#define p(i)	(i/2-1)
#define l(i)	(2*i+1)
#define r(i)	l(i)+1

void maxHeapifyDown(int a[],int len,int elem_idx)
{
	int p = elem_idx;
	int biggest_child = r(p);
	int l = biggest_child - 1;
	while(l < len) {
		biggest_child -= (biggest_child>=len || a[l] > a[biggest_child]);
		if(a[biggest_child] > a[p]) {
			swapElem(a,biggest_child,p);
			p = biggest_child;
			biggest_child = r(p);
			l = biggest_child - 1;
		}else {
			break;
		}
	}
}

void heapSort(int a[],int len)
{
	int i;
	for(i=len/2;i>=0;i--) {
		maxHeapifyDown(a,len,i);
	}

	int _len = len;
	while(_len > 0) {
		swapElem(a,0,--_len);
		maxHeapifyDown(a,_len,0);
	}
}

int quickPartition(int a[],int s,int e)
{
	int pivot = a[e];
	int i;
	int last_elem_lt_pivot = -1;
	for(i=s;i<e;i++) {
		if(a[i] < pivot) {
			swapElem(a,i,++last_elem_lt_pivot);
		}
	}
	swapElem(a,e,++last_elem_lt_pivot);
	return last_elem_lt_pivot;
}

void quickSort(int a[],int len)
{
	if(len == 0) {
		return;
	}

	int pivot_idx = quickPartition(a,0,len-1);
	quickSort(a,pivot_idx);
	quickSort(a+pivot_idx+1,len-pivot_idx-1);
}

void bubbleSort(int a[],int len)
{
	int i;
	for(i=0;i<len;i++) {
		int j;
		for(j=len-1;j>i;j--) {
			if(a[j] < a[j-1]) {
				swapElem(a,j,j-1);
			}
		}
	}
}

inline int len_of_num(int a)
{
	int l = 0;
	int i = a;
	do {
		l++;
		i /= 10;
	} while(i != 0);
	return l;
}

int biggest_len(int a[],int len)
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

void radixSort(int a[],int len)
{
	int loops = biggest_len(a,len);
	int *x = (int*)malloc(sizeof(int)*len);
	int *y = (int*)malloc(sizeof(int)*len);
	memcpy(x,a,len*sizeof(int));
	int *p = x;
	int *q = y;
	int count[10];

	int i = 0;
	while(i < loops) {

		int j;

		for(j=0;j<10;j++) {
			count[j] = 0;
		}

		for(j=0;j<len;j++) {
//			int bit_of_num = low_bit_of_num(p[j],i);
//			printf("p[%d] = %d -- bit_of_num = %d\n",j,p[j],bit_of_num);
			count[low_bit_of_num(p[j],i)]++;
		}

		count[9] = len - count[9];
		for(j=8;j>=0;j--) {
			count[j] = count[j+1] - count[j];
		}

		for(j=0;j<len;j++) {
			q[count[low_bit_of_num(p[j],i)]++] = p[j];
		}

		int *tmp = p;
		p = q;
		q = tmp;
		i++;
	}

	/* copy back */
	memcpy(a,p,len*sizeof(int));

	free(x);
	free(y);
}

void bucketSort(int a[],int len)
{
}

void mergeLR(int l[],int llen,int r[],int rlen)
{
	int total_len = llen + rlen;
	int *tp = l;

	int *tmp = (int*)malloc(sizeof(int)*total_len);

	int i = 0,j = 0,ti = 0;

	while(i < llen && j < rlen) {
		if(l[i] < r[j]) {
			tmp[ti++] = l[i++];
		}else {
			tmp[ti++] = r[j++];
		}
	}

	while(i < llen) {
		tmp[ti++] = l[i++];
	}

	while(j < rlen) {
		tmp[ti++] = r[j++];
	}

	memcpy(tp,tmp,total_len*sizeof(int));

	free(tmp);
}

void mergeSort(int a[],int len)
{
	if(len <= 1) {
		return;
	}

	int llen = len/2;
	int rlen = len - llen;

	int *lp = a;
	int *rp = a + llen;

	mergeSort(lp,llen);
	mergeSort(rp,rlen);

	mergeLR(lp,llen,rp,rlen);
}

void countingSort(int a[],int len)
{
	int clen = max_of_array(a,len) + 1;

	int *c = (int*)malloc(sizeof(int)*clen);
	int *b = (int*)malloc(sizeof(int)*len);

	int i;
	for(i=0;i<clen;i++) {
		c[i] = 0;
	}

	for(i=0;i<len;i++) {
		c[a[i]]++;
	}

	for(i=1;i<clen;i++) {
		c[i] += c[i-1];
	}

	for(i=0;i<len;i++) {
		b[--c[a[i]]] = a[i];
	}

	memcpy(a,b,sizeof(int)*len);

	free(c);
	free(b);
}

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
//	quickSort(a,ELEM_N);
//	heapSort(a,ELEM_N);
//	radixSort(a,ELEM_N);
//	countingSort(a,ELEM_N);
	mergeSort(a,ELEM_N);
	printIntarray(a,ELEM_N);

	return 0;
}
