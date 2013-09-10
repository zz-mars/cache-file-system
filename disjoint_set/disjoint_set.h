#ifndef _DISJOINT_SET_H
#define _DISJOINT_SET_H

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

typedef struct {
	int i;
	int set;
	int rank;
} elem_t;

static inline void print_elems(elem_t *els,int n)
{
	int i;
	for(i=0;i<n;i++) {
		printf("#%2d - i #%2d set - #%2d rank - #%2d\n",
				i,els[i].i,els[i].set,els[i].rank);
	}
	printf("----------------------------------\n");
}

void make_set(elem_t *el,int i);

int find_set(elem_t *el);

void Union(elem_t *x,elem_t *y);

#endif
