#include "disjoint_set.h"

void make_set(elem_t *el,int i)
{
	el->set = el->i = i;
	el->rank = 0;
}

int find_set(elem_t *el)
{
	if(el->i != el->set) {
		el->set = find_set(el+el->set-el->i);
	}
	return el->set;
}

void Union(elem_t *i,elem_t *j)
{
	int si = find_set(i);
	int sj = find_set(j);

	if(si == sj) {
		return;
	}

	elem_t *s = i - i->i;
	elem_t *s1 = j - j->i;

	if(s != s1) {
		fprintf(stderr,"fatal error!\n");
		return;
	}

	if(s[si].rank > s[sj].rank) {
		s[sj].set = si;
	} else {
		s[si].set = sj;
		if(s[si].rank == s[sj].rank) {
			s[sj].rank++;
		}
	}
}

/*
#define ELEM_N	10

int main()
{
	elem_t elems[ELEM_N];

	int i;
	for(i=0;i<ELEM_N;i++) {
		make_set(&elems[i],i);
	}
	
	print_elems(&elems[0],ELEM_N);

	Union(&elems[1],&elems[3]);
	Union(&elems[4],&elems[2]);

	Union(&elems[1],&elems[2]);

	Union(&elems[5],&elems[2]);

	Union(&elems[6],&elems[2]);
	Union(&elems[8],&elems[6]);

	Union(&elems[5],&elems[6]);

	Union(&elems[6],&elems[9]);

	Union(&elems[4],&elems[7]);
	Union(&elems[7],&elems[9]);

	for(i=0;i<ELEM_N;i++) {
		find_set(&elems[i]);
	}

	print_elems(&elems[0],ELEM_N);

	return 0;
}

*/
