#include <stdio.h>

void do_stuff(void)
{
	printf("Hello,world!\n");
}

int main()
{
	int i;
	for(i=0;i<4;i++) {
		do_stuff();
	}
	return 0;
}
