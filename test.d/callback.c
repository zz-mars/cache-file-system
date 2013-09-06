#include <stdio.h>
typedef void (*callback_t)(void*arg);

void repeat_3_times(callback_t cb,void *arg) 
{
	int i;
	for(i=0;i<3;i++) {
		cb(arg);
	}
}

void count_numer(void *arg)
{
	int i = *(int*)arg;
	int j;
	for(j=0;j<i;j++) {
		printf("%d\n",j);
	}
}

void say_hello(void *arg)
{
	printf("%s\n",(char*)arg);
}

int main()
{
	int i = 3;
	char *s = "hello world!";
	repeat_3_times(count_numer,&i);
	repeat_3_times(say_hello,s);
	return 0;
}
