#include"glob.h"
#include"rsync.h"
#define CBUFSZ	(BLK_SZ+DELTA_BLOCK_ENTRY_SZ+1)
int main(int argc,char * argv[])
{
	int fd,n;
	pid_t pid;
	u8 buf[CBUFSZ];
	struct stat cstat;
	chunk_file_header cfh;
	chunk_block_entry cblk;
	delta_block_entry dblk;
	u64 d_off;
	u32 d_old_len,d_new_len;
	u8 d_embeded;
	u32 block_sz;
	u64 block_nr;
	u64 c_off,file_sz,i,remaining_bytes;
	u32 c_len;
	u8 * file_name;
    int connfd,clen;
    int res;
    struct sockaddr_in addr;
	if(argc != 2){
		fprintf(stderr,"invalid argument!\n");
		exit(1);
	}
	file_name = argv[1];
	fd = open(file_name,O_RDWR);
	if(fd < 0){
		perror("open");
		exit(1);
	}
	fstat(fd,&cstat);
	file_sz = cstat.st_size;
	bzero(&cfh,CHUNK_FILE_HEAD_SZ);
	block_sz = BLK_SZ;
	block_nr = file_sz/BLK_SZ;
	if(file_sz%BLK_SZ != 0){
		block_nr += 1;
	}
	cfh.block_nr = block_nr;
	cfh.block_sz = block_sz;
	strncpy(cfh.fn,file_name,FILE_NAME_LEN);
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,RSYNC_HOST_IP,(void *)&addr.sin_addr);
    addr.sin_port = htons(RSYNC_SRV_PORT);
    clen = sizeof(addr);
    if((connfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        perror("Socket");
		goto over1;
    }
    if(connect(connfd,(struct sockaddr*)&addr,clen) == -1){
        perror("Connection");
		goto over1;
    }
	if((pid = fork()) == 0){
		/* child process receive the delta blk */
		while((n = read(connfd,&dblk,DELTA_BLOCK_ENTRY_SZ)) == DELTA_BLOCK_ENTRY_SZ){
			d_embeded = dblk.embeded;
			d_off = dblk.offset;
			d_old_len = dblk.old_len;
			d_new_len = dblk.new_len;
			if(d_embeded == BLK_SAME){
				/* no need to sync ,just continue */
				continue;
			}
			if(d_old_len > d_new_len){
				/* need truncate */
				ftruncate(fd,d_off + d_new_len);
				if(d_new_len == 0){
					break;
				}
			}
			/* need overwrite */
			bzero(buf,CBUFSZ);
			if((n = read(connfd,buf,d_new_len)) != d_new_len){
				/* something is wrong */
				perror("read from socket");
				exit(1);
			}
			lseek(fd,d_off,SEEK_SET);
			if((n = write(fd,buf,d_new_len)) != d_new_len){
				/* something is wrong */
				perror("write to file");
				exit(1);
			}
		}
		close(connfd);
		close(fd);
		exit(0);
	}
	if((n = write(connfd,&cfh,CHUNK_FILE_HEAD_SZ)) != CHUNK_FILE_HEAD_SZ){
		perror("writesocket");
		goto over1;
	}
	for(i = 0;i < block_nr;i++){
		c_off = i*block_sz;
		remaining_bytes = file_sz - c_off;
		c_len = (remaining_bytes<block_sz?remaining_bytes:block_sz);
		cblk.offset = c_off;
		cblk.len = c_len;
		bzero(buf,CBUFSZ);
		if((n = read(fd,buf,c_len)) != c_len){
			fprintf(stderr,"read file fail!\n");
			goto over1;
		}
		cal_md5(buf,c_len,cblk.md5);
		if((n = write(connfd,&cblk,CHUNK_BLOCK_ENTRY_SZ)) != CHUNK_BLOCK_ENTRY_SZ){
			fprintf(stderr,"write socket...\n");
			goto over1;
		}
	}
over1:
	close(fd);
    return 0;
}
