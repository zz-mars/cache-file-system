#include"glob.h"
#include"rsync.h"
static u8 buf[BUFSIZ];
static u8 * ptr = buf + CHUNK_BLOCK_ENTRY_SZ;
int main()
{
	chunk_block_entry cblk;
	u8 smd5[MD5_SZ+1];
	delta_block_entry dblk;
	u32 sz_to_send,n,out_of_loop_value;
	u64 serv_off,serv_file_sz,remaining_bytes;
	int fd;
	struct stat f_st;
	u32 clnt_blksz;
	u64 clnt_blknr;
	u8 file_name[FILE_NAME_LEN];
	int port;
    int listenfd,connfd,len;
    char ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in addr,cli_addr;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,RSYNC_HOST_IP,(void*)&addr.sin_addr);
    addr.sin_port = htons(RSYNC_SRV_PORT);
    len = sizeof(addr);
    bind(listenfd,(struct sockaddr *)&addr,len);
    listen(listenfd,LISTENQ);
	len = sizeof(cli_addr);
	connfd = accept(listenfd,(struct sockaddr *)&cli_addr,&len);
	bzero(ip_str,INET_ADDRSTRLEN);
	inet_ntop(AF_INET,(void*)&cli_addr.sin_addr,ip_str,INET_ADDRSTRLEN);
	port = ntohs(cli_addr.sin_port);
	printf(" from %s,port %d\n",ip_str,port);
	/*-------------- read chunk header ---------*/
	bzero(buf,BUFSIZ);
	n = read(connfd,buf,CHUNK_FILE_HEAD_SZ);
	if(n != CHUNK_FILE_HEAD_SZ){
		fprintf(stderr,"read chunk_file_head fail!\n");
		goto over2;
	}
	/* init from chunk header */
	clnt_blksz = ((chunk_file_header *)buf)->block_sz;
	clnt_blknr = ((chunk_file_header *)buf)->block_nr;
	bzero(file_name,FILE_NAME_LEN);
	strncpy(file_name,((chunk_file_header*)buf)->fn,FILE_NAME_LEN);
	printf("clnt_blksz   -- %d\n",clnt_blksz);
	printf("clnt_blknr   -- %ld\n",clnt_blknr);
	printf("file to sync -- %s\n",file_name);
	fd = open(file_name,O_RDONLY);
	if(fd < 0){
		perror("open");
		goto over2;
	}
	lseek(fd,0,SEEK_SET);
	if(fstat(fd,&f_st) != 0){
		perror("fstat");
		goto over1;
	}
	serv_file_sz = f_st.st_size;
	if(clnt_blknr == 0){
		cblk.offset = 0;
		goto overwrite;
	}
	while((n = read(connfd,&cblk,CHUNK_BLOCK_ENTRY_SZ)) == CHUNK_BLOCK_ENTRY_SZ){
		out_of_loop_value = 0;
		sz_to_send = DELTA_BLOCK_ENTRY_SZ;
		dblk.offset = cblk.offset;
		dblk.old_len = cblk.len;
		dblk.embeded = BLK_SAME;
		if(cblk.offset == serv_file_sz){
			/*-------- case 1 --------*/
			/* out of boundry,
			 * truncate it */
			printf("case 1 --------------- need truncate..........\n");
			dblk.new_len = 0;
			dblk.embeded = BLK_DIFF;
			out_of_loop_value = 1;//for break
		}else if(cblk.offset < serv_file_sz && cblk.len == clnt_blksz){
			lseek(fd,cblk.offset,SEEK_SET);
			remaining_bytes = serv_file_sz - cblk.offset;
			if(remaining_bytes >= clnt_blksz){
			/* the most common case */
				/*-------- case 2 ---------*/
				dblk.new_len = clnt_blksz;
				n = read(fd,ptr,clnt_blksz);
				cal_md5(ptr,clnt_blksz,smd5);
				if(!MD5_EQUAL(cblk.md5,smd5)){
					printf("case 2  ----------------------------------------md5 different!!!!\n");
					dblk.embeded = BLK_DIFF;
					sz_to_send += n;
				}
				printf("case 2  ----------------------------------------md5 same!!!!\n");
				/* no append data */
			}else if(remaining_bytes < clnt_blksz){
				/*---------- case 3 ---------*/
				/* remaining_bytes are less than clnt_blksz,
				 * truncate */
				printf("case 3 ------------------------ need truncate\n");
				n = read(fd,ptr,remaining_bytes);
				dblk.new_len = remaining_bytes;
				dblk.embeded = BLK_DIFF;
				sz_to_send += n;
				out_of_loop_value = 3;
			}
		}else if(cblk.len < clnt_blksz){
			/* last blk of client */
			lseek(fd,cblk.offset,SEEK_SET);
			remaining_bytes = serv_file_sz - cblk.offset;
			if(remaining_bytes <= cblk.len){
				/*--------- case 4 ---------*/
				/* truncate */
				if(remaining_bytes == cblk.len){
					printf("case 4 ------------------ totally indentical file size,no need truncate \n");
				}else{
					printf("case 4 ------------------------ need truncate\n");
				}
				n = read(fd,ptr,remaining_bytes);
				dblk.new_len = remaining_bytes;
				dblk.embeded = BLK_DIFF;
				sz_to_send += n;
				out_of_loop_value = 4;
			}else{
				/*---------- case 5 --------*/
				/* just overwrite from this blk */
				printf("case 5 ------------------------ need overwrite \n");
				out_of_loop_value = 5;
				break;
			}
		}
		memcpy(buf,&dblk,DELTA_BLOCK_ENTRY_SZ);
//		if((n = write(connfd,buf,sz_to_send)) != sz_to_send){
//			perror("write to socket");
//			goto over1;
//		}
		if(out_of_loop_value != 0){break;}
	}
	if(out_of_loop_value != 5){
		goto over1;
	}
overwrite:
	serv_off = cblk.offset;
	printf("overwrite client replica from offset : %d\n",serv_off);
	while(serv_off < serv_file_sz){
		lseek(fd,serv_off,SEEK_SET);
		dblk.offset = serv_off;
		dblk.old_len = 0;
		dblk.embeded = BLK_DIFF;
		remaining_bytes = serv_file_sz - serv_off;
		dblk.new_len = (remaining_bytes >= clnt_blksz?clnt_blksz:remaining_bytes);
		sz_to_send = dblk.new_len + DELTA_BLOCK_ENTRY_SZ;
		n = read(fd,ptr,dblk.new_len);
		memcpy(buf,&dblk,DELTA_BLOCK_ENTRY_SZ);
		n = write(connfd,buf,sz_to_send);
		serv_off += dblk.new_len;
	}
over1:
	close(fd);
over2:
	close(connfd);
	return 0;
}
