#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "md5.h"

#define BLK_SZ	(4*1024)

int main(int argc,char *argv[])
{
	if(argc != 2) {
		fprintf(stderr,"invalid arg!\n");
		return 1;
	}

	char *file = argv[1];

	int fd = open(file,O_RDONLY);
	if(fd < 0) {
		perror("open file for read");
		return 1;
	}
	lseek(fd,0,SEEK_SET);

	char buf[BLK_SZ];
	int n;
	while(1) {
		n = read(fd,buf,BLK_SZ);
		if(n == -1) {
			perror("read file");
			break;
		}
		char md5[MD5_STRING_LEN];
		memset(md5,0,MD5_STRING_LEN);
		md5s_of_str(buf,n,md5);
//		printf("%s\n",md5);

		char cmd[128];
		memset(cmd,0,128);

		snprintf(cmd,128,"redis-cli incr %s | awk '{print $1}'",md5);
		FILE *fp = popen(cmd,"r");
		if(!fp) {
			perror("fopen");
			return 1;
		}

		char reply[32];
		memset(reply,0,32);
		fgets(reply,32,fp);

		pclose(fp);

//		printf("reply %s\n",reply);

		int ref_count = 0;

		char *p = reply;
		if(*p < '0' || *p > '9') {
			fprintf(stderr,"invalid character found!\n");
			return 1;
		}

		while(*p >= '0' && *p <= '9') {
			ref_count *= 10;
			ref_count += *p-'0';
			p++;
		}

//		printf("blk %s -- ref_count %d\n",md5,ref_count);

		if(ref_count == 1) {
			/* write to cache */
			char fn[128];
			memset(fn,0,128);
			snprintf(fn,128,"/mnt/supercache/%s",md5);

//			printf("blk file %s\n",fn);

			int _fd = open(fn,O_CREAT | O_WRONLY,0660);
			if(_fd < 0) {
				perror("open block file");
				return 1;
			}

			if(write(_fd,buf,n) != n) {
				perror("write to block file");
				return 1;
			}

			close(_fd);
		}

		if(n < BLK_SZ) {
			break;
		}
	}

	close(fd);
	return 0;
}

