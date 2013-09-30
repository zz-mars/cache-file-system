#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//#define DBGMSG
#define ZMAX_INT	0x7fffffff
#define MATRIX_N	6

#define MS_MATRIX_L	(MATRIX_N*MATRIX_N)

static unsigned int ms_matrix[MS_MATRIX_L];
static unsigned int p[MATRIX_N+1] = {30,35,15,5,10,20,25};

#define INDEX_OF(i,j)	(((i)-1)*MATRIX_N+(j)-1)
#define ZSET(i,j,v)		ms_matrix[INDEX_OF(i,j)] = v
/*
#define MSET(i,j,v)		ms_matrix[INDEX_OF(i,j)] = v
#define SSET(i,j,v)		ms_matrix[INDEX_OF(j,i)] = v
*/

#define M(i,j)			ms_matrix[INDEX_OF(i,j)]
#define S(i,j)			ms_matrix[INDEX_OF(j,i)]

static void init_ms_matrix(void)
{
	int i,j;
	memset(ms_matrix,0,sizeof(int)*MS_MATRIX_L);
	for(i=1;i<=MATRIX_N;i++){
		for(j=1;j<=MATRIX_N;j++){
			if(i<j){
				ZSET(i,j,ZMAX_INT);
			}
		}
	}
}

static void prt_ms_matrix(void)
{
	int i,j;
	for(i=1;i<=MATRIX_N;i++){
		for(j=1;j<=MATRIX_N;j++){
			printf("%3d ",M(i,j));
		}
		printf("\n");
	}
	printf("--------------------\n");
}

static void prt_optimal_solution(int i,int j)
{
	int k;
	if(i == j){
		printf("a%d",i);
		return;
	}
	k = S(i,j);
	printf("(");
	prt_optimal_solution(i,k);
	prt_optimal_solution(k+1,j);
	printf(")");
}

static int recur_depth = 0;

static int multi_chain_matrix_recursive_style(int i,int j)
{
	if(i > j) {
		fprintf(stderr,"invalid arg!\n");
		return -1;
	}

	if(M(i,j) < ZMAX_INT) {
		goto ret;
	}

	if(i==j){
		M(i,j) = 0;
	}else if((i+1) == j){
		M(i,j) = p[i-1]*p[i]*p[i+1];
		S(i,j) = i;
	}else {
		int k;
		recur_depth++;
		for(k=i;k<j;k++){
			unsigned int v = p[i-1]*p[k]*p[j] +	\
							 multi_chain_matrix_recursive_style(i,k) +	\
							 multi_chain_matrix_recursive_style(k+1,j);
			if(v < M(i,j)){
				M(i,j) = v;
				S(i,j) = k;
			}
		}
	}
ret:
	return M(i,j);
}

static void multi_chain_matrix(void)
{
	int i,j,k,v,l;
	for(l=2;l<=MATRIX_N;l++){
#ifdef DBGMSG
		printf("--------- length #%d ----------\n",l);
#endif
		for(i=1;i<=MATRIX_N-l+1;i++){
			j = i+l-1;
#ifdef DBGMSG
			printf("---- i#%d\tj#%d ----\n",i,j);
#endif
			for(k=i;k<j;k++){
#ifdef DBGMSG
				printf("k#%d\n",k);
#endif
				v = M(i,k)+M(k+1,j)+p[i-1]*p[k]*p[j];
#ifdef DBGMSG
				printf("M(%d,%d) #%d\n",i,j,M(i,j));
				printf("S(%d,%d) #%d\n",i,j,S(i,j));
				printf("v = M(i,k)+M(k+1,j)+p[i-1]*p[k]*p[j] == #%d\n",v);
#endif
				if(v < M(i,j)){
					M(i,j) = v;
					S(i,j) = k;
				}
#ifdef DBGMSG
				printf("new M(%d,%d) #%d\n",i,j,M(i,j));
				printf("new S(%d,%d) #%d\n",i,j,S(i,j));
#endif
			}
		}
	}
	return;
}
int main()
{
	init_ms_matrix();
	prt_ms_matrix();
	multi_chain_matrix();
	prt_ms_matrix();
	prt_optimal_solution(1,MATRIX_N);
	printf("\n");
	printf("------------------recursive style---------------\n");
	init_ms_matrix();
	prt_ms_matrix();
	multi_chain_matrix_recursive_style(1,MATRIX_N);
	printf("recur_depth #%d\n",recur_depth);
	prt_ms_matrix();
	prt_optimal_solution(1,MATRIX_N);
	printf("\n");
	return;
}
