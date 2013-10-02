#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define minv(x,y)	({	\
		typeof(x) _x = x;	\
		typeof(y) _y = y;	\
		_x<_y?_x:_y;	\
		})

int longest_palindrome_sub_string(char *str,unsigned int *lps,unsigned int *idx)
{
	if(!lps || !idx) {
		return 1;
	}

	unsigned int _len = strlen(str);
	if(_len == 0) {
		*lps = *idx = 0;
		return 0;
	}

	int len = _len * 2 + 1;

	char *s = (char*)malloc(len);
	if(!s) {
		return 1;
	}

	unsigned int *p = (unsigned int*)malloc(sizeof(unsigned int)*len);
	if(!p) {
		free(s);
		return 1;
	}
	memset(p,0,sizeof(unsigned int)*len);

	int i;
	for(i=0;i<_len;i++) {
		s[2*i] = '#';
		s[2*i+1] = str[i];
	}
	s[len-1] = '#';

	for(i=0;i<len;i++) {
		printf("%c ",s[i]);
	}
	printf("\n");

	unsigned int id = 0;
	unsigned int mx = 0;

	for(i=0;i<len;i++) {

		p[i] = mx>i?minv(p[2*id-i],mx-i):1;
		
		unsigned int lidx = i - p[i];
		unsigned int ridx = i + p[i];

		while(lidx >= 0 && ridx < len && s[lidx] == s[ridx]) {
			p[i]++;
			lidx = i - p[i];
			ridx = i + p[i];
		}

		if(i+p[i] > mx) {
			mx = i + p[i];
			id = i;
		}
	}

	unsigned int _idx = 0;
	unsigned int _lps = 0;

	for(i=0;i<len;i++) {
		if(p[i] > _lps) {
			_lps = p[i];
			_idx = i;
		}
		printf("%d ",p[i]);
	}
	printf("\n");

	*lps = _lps - 1;
	*idx = (_idx-_lps)/2 + 1;

	free(s);
	free(p);

	return 0;
}

int main(int argc,char *argv[])
{
	if(argc != 2) {
		fprintf(stderr,"invalid arg!\n");
		return 1;
	}

	char *s = argv[1];
//	char *s = "waabwswfd";

	int lps,idx;
	if(longest_palindrome_sub_string(s,&lps,&idx)) {
		perror("err");
		return 1;
	}
	
	printf("lps %d	idx %d\n",lps,idx);

	return 0;
}
