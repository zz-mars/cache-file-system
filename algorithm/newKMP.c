#include "glob.h"

void init_partial_match_table(char *patern,int pat_len,int pmt[])
{
	int i;
	char *tmpbuf = (char*)malloc(pat_len*2);
	for(i=2;i<=pat_len;i++) {
		int j;
		pmt[i-1] = 0;
		for(j=1;j<i;j++) {
			char *x = tmpbuf;
			char *y = tmpbuf+j+1;
			strncpy(x,patern,j);
			strncpy(y,patern+i-j,j);
			if(strncmp(x,y,j) == 0) {
				pmt[i-1] = j;
			}
		}
	}
	pmt[0] = 0;
	free(tmpbuf);
}

int find_patern(char *tocheck,char *patern,int pat_len,int pmt[],int *pos)
{
	int tocheck_idx = 0;
	int pat_idx = 0;
	int tocheck_len = strlen(tocheck);
	while(pat_idx != pat_len && tocheck_idx != tocheck_len) {
		sleep(1);
		printf("pat_idx #%d %c -- tocheck_idx #%d %c\n",pat_idx,patern[pat_idx],tocheck_idx,tocheck[tocheck_idx]);
		if(tocheck[tocheck_idx] == patern[pat_idx]) {
			pat_idx++;
			tocheck_idx++;
			continue;
		}
		if(pat_idx > 0) {
			pat_idx = pmt[pat_idx-1];
		}else{
			tocheck_idx++;
		}
	}

	if(pat_idx == pat_len) {
		*pos = tocheck_idx - pat_len;
		return 1;
	}

	return 0;
}

void print_string_with_space(char *s,int pos,int len)
{
	int i;
	int slen = strlen(s);
	if(slen <= pos || slen < (pos + len)) {
		fprintf(stderr,"out of range!\n");
		return;
	}

	for(i=0;i<pos;i++) {
		printf("%c",s[i]);
	}
	printf(" ");
	for(i=pos;i<pos+len;i++) {
		printf("%c",s[i]);
	}
	printf(" ");
	printf("%s\n",s+i);
}

int main(int argc,char *argv[])
{
	if(argc != 3) {
		fprintf(stderr,"invalid argument!\n");
		return 1;
	}

	char *patern = argv[1];
	char *tocheck = argv[2];

	int pat_len = strlen(patern);
	int *pmt = (int*)malloc(sizeof(int)*pat_len);
	init_partial_match_table(patern,pat_len,pmt);
	int i;
	printf("partial match table:\n");
	for(i=0;i<pat_len;i++) {
		printf("%d ",pmt[i]);
	}
	printf("\n");

	int pos;
	if(find_patern(tocheck,patern,pat_len,pmt,&pos)) {
		print_string_with_space(tocheck,pos,pat_len);
	}

	free(pmt);
	return 0;
}
