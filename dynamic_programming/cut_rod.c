#include <stdio.h>

#define ELEM_N	10

static int recurse_depth = 0;

static int p[ELEM_N] = {1,5,8,9,10,17,17,20,24,30};
static int r[ELEM_N] = {-1};

inline int zmax(int a,int b)
{
	return (a>=b?a:b);
}

int best_solution_of_rod(int len)
{
	if(len > ELEM_N) {
		fprintf(stderr,"invalid argument!\n");
		return -1;
	}

	if(len == 0) {
		return 0;
	}

	if(r[len-1] != -1) {
		return r[len-1];
	}
	
	recurse_depth++;

	int i;
	for(i=1;i<=len;i++) {
		r[len-1] = zmax(r[len-1],p[i-1] + best_solution_of_rod(len-i));
	}

	return r[len-1];
}

int cut_rod(int len)
{
	return best_solution_of_rod(len);
}

inline void init_r(void)
{
	int i;
	for(i=0;i<ELEM_N;i++) {
		r[i] = -1;
	}
	recurse_depth = 0;
}

int main()
{
	init_r();

	int pi = cut_rod(10);

	printf("cut_rod is called #%d times!best solution %d\n",recurse_depth,pi);

	return 0;
}

