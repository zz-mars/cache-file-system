#include <stdio.h>

#define minv(x,y)	({	\
		typeof(x) _x = x;	\
		typeof(y) _y = y;	\
		_x<_y?_x:_y;	\
		})

#define ELEM_N	128

char ss[ELEM_N] = "waabwswfd";

int longest_palindrome_sub_string(char *str)
{
	unsigned int _len = strlen(str);
	if(_len == 0) {
		return 0;
	}

	int len = _len * 2 + 1;

	char *s = (char*)malloc(len);
	if(!s) {
		return -1;
	}

	unsigned int *p = (unsigned int*)malloc(sizeof(unsigned int)*len);
	if(!p) {
		free(s);
		return -1;
	}
	memset(p,0,sizeof(int)*len);

	int i;
	for(i=0;i<_len;i++) {
		s[2*i] = '#';
		s[2*i+1] = str[i];
	}
	s[len-1] = '#';

	printf("%s\n",s);

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

	free(s);
	free(p);

	return;
}

