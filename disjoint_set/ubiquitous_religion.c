#include "disjoint_set.h"

int main()
{
	int n,m,c=0;
	int i,j,k;

	scanf("%d%d",&n,&m);

	while(!(n == 0 && m == 0)){

		elem_t *elems = (elem_t*)malloc(sizeof(elem_t)*n);

		for(i=0;i<n;i++){
			make_set(&elems[i],i);
		}

		for(i=0;i<m;i++){
			scanf("%d%d",&j,&k);
			if(j > n || k > n){
				fprintf(stderr,"invalid\n");
				exit(1);
			}
			Union(&elems[j-1],&elems[k-1]);
		}

		j=0;
		for(i=0;i<n;i++){
			if(elems[i].set == elems[i].i){
				j++;
			}
		}

		free(elems);

		printf("case %d: %d\n",++c,j);
		scanf("%d%d",&n,&m);
	}
	return 0;
}
