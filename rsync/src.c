#include"glob.h"
#include"rsync.h"
#define CHUNK_BLK_HASH	256
#define EVERY_READ_BYTES	1024
static u8 buf[BUFSIZ];
static int buf_file_fd;/* opened fd of buffered file */
static u64 buf_file_sz;/* file size */
static u64 buf_file_off;/* corresponding file offset */
static u32 buf_bytes;/* total bytes in buffer */
//static u32 current_buf_pos;/* current buffer position */
//static u32 unread_buf_bytes;/* bytes that haven't been read */
static u8 * ptr = buf + DELTA_BLOCK_ENTRY_SZ;
static list_head * chunk_blk_entry_hash[CHUNK_BLK_HASH] = {NULL};
#define ROLLING_CHECKSUM_HASH(rolling_chksm)	((rolling_chksm)%CHUNK_BLK_HASH)
static delta_block_entry * new_dblk_ent()
{
	delta_block_entry * dblk = (delta_block_entry*)malloc(DELTA_BLOCK_ENTRY_SZ);
	if(dblk == NULL){
		perror("malloc for a new delta_block_entry");
		return NULL;
	}
	dblk->dbe_list.pre = &(dblk->dbe_list);
	dblk->dbe_list.next = &(dblk->dbe_list);
	return dblk;
}
static void buf_init(int fd,u64 file_sz)
{
	buf_file_fd = fd;
	buf_file_sz = file_sz;
	buf_file_off = 0;
	buf_bytes = 0;
	return;
}
static u32 fill_buf(u64 off,u32 len)
{
	/* fill buffer with data whose offset is 'off'
	 *								length is 'len'
	 * return value is the bytes filled into buffer.
	 * */
	u32 n;
	lseek(buf_file_fd,off,SEEK_SET);
	bzero(buf,BUFSIZ);
	n = read(buf_file_fd,buf,len);
	if((n == len) || \
			(n < len && (n + off) == buf_file_sz)){
		/* accepted case */
		buf_file_off = off;
		buf_bytes = n;
		return 0;
	}
	return 1;
}
static u8 * read_from_buf(u64 offset,u32 bytes,u32 * n)
{
	u64 buf_first_byte;
	u64 buf_last_byte;
	u64 req_first_byte;
	u64 req_last_byte;
retry_read_from_buf:
	buf_first_byte = buf_file_off;
	buf_last_byte = buf_file_off + buf_bytes;
	req_first_byte = offset;
	req_last_byte = offset + bytes;
	if(req_last_byte <= buf_last_byte && req_first_byte >= buf_first_byte){
		/* ------------------- accepted case 0 ---------------------*/
		*n = bytes;
		return (buf + req_first_byte - buf_first_byte);
	}
	if(req_first_byte >= buf_first_byte && \
			req_first_byte < buf_last_byte && \
			req_last_byte >= buf_last_byte && \
			buf_last_byte == file_sz){
		/*------------------ accepted case 1 --------------------*/
		*n = buf_last_byte - req_first_byte;
		return (buf + req_first_byte - buf_first_byte);
	}
	/* ---------- unaccepted case --------- */
	if(fill_buf(offset,EVERY_READ_BYTES) == 0)
		goto retry_read_from_buf;
	return NULL;
}
int main()
{
	/* rsync variables */
	u8 file_name[FILE_NAME_LEN] = {'\0'};
	u32 block_sz;
	u32 i;
	list_head ** lst_hd,**lh_pre,**lh_next;
	u32 cblk_hash_v;
	/* chunk block */
	u64 chunk_block_nr;
	chunk_file_header cfh;
	chunk_block_entry * cblk;
	chunk_block_entry * cblk_array;
	chunk_block_entry * cblk_p;
	chunk_block_entry * cblk_current;
	u64 chunk_block_no;
	u32 brk_v;
	u32 is_first_entry,is_last_entry;
	/* delta block */
	u64 delta_block_nr;
	delta_file_header dfh;
	delta_block_entry * dblk;
	list_head * dblk_link_list_head = NULL;
	list_head * dblk_link_list_tail = NULL;
	delta_block_entry * dblk_p;
	/* searching for dup blocks */
	u64 remaining_bytes;
	u64 delta_region_start,delta_region_end;
	u64 dup_blk_off;
	u32 dup_blk_len;
	u32 rolling_chksm;
	u8 md5[MD5_SZ];
	/* file to sync */
	int fd;
	struct stat file_stat;
	u64 file_sz;
	/* socket variables */
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
	if(Read(connfd,&cfh,CHUNK_FILE_HEAD_SZ) != CHUNK_FILE_HEAD_SZ){
		perror("read chunk_file_header");
		goto over1;
	}
	printf("--------- read chunk_blk_header ok -----------\n");
	block_sz = cfh.block_sz;
	chunk_block_nr = cfh.block_nr;
	strncpy(file_name,cfh.fn,strlen(cfh.fn));
	printf("---	block_sz		-- # %d\n",block_sz);
	printf("---	block_nr		-- # %d\n",chunk_block_nr);
	printf("---	file_to_sync	-- # %s\n",file_name);
	if((fd = open(file_name,O_RDONLY)) < 0){
		perror("open src file");
		if(errno == ENOENT)
			dfh.err = E_FILE_NOT_EXIST;
			goto send_delta_file_header;
		goto over1;
	}
	if(fstat(fd,&file_stat) != 0){
		perror("fstat");
		goto over2;
	}
	file_sz = file_stat.st_size;
	/* dst file has 0 block */
	if(chunk_block_nr == 0){
		printf("dst file has no block!\n");
	}
	cblk_array = (chunk_block_entry*)malloc(chunk_block_nr*CHUNK_BLOCK_ENTRY_SZ);
	if(cblk_array == NULL){
		perror("malloc for chunk_block_entry_array");
		goto over2;
	}
	for(i = 0;i < chunk_block_nr;i++){
		cblk_p = cblk_array + i;
		if(Read(connfd,cblk_p,CHUNK_BLOCK_ENTRY_SZ) != CHUNK_BLOCK_ENTRY_SZ){
			perror("read chunk_block_entry");
			goto over3;
		}
		chunk_block_no = cblk_p->block_no;
		cblk_p->cbe_hash.next = &(cblk_p->cbe_hash);
		cblk_p->cbe_hash.pre = &(cblk_p->cbe_hash);
		cblk_hash_v = ROLLING_CHECKSUM_HASH(cblk_p->rolling_chksm);
		lst_hd = &chunk_blk_entry_hash[cblk_hash_v];
		if(*lst_hd == NULL){
			*lst_hd = &(cblk_p->cbe_hash);
			continue;
		}
		while(1){
			brk_v = 0;
			cblk_current = containerof(*lst_hd,chunk_block_entry,cbe_hash);
			if(cblk_current->block_no >= chunk_block_no){
				brk_v = 1;
				break;
			}
			if(IS_LAST_ENTRY(*lst_hd)){
				brk_v = 2;
				break;
			}
			lst_hd = &((*lst_hd)->next);
		}
		switch(brk_v){
			case 1:
				is_first_entry = 0;
				if(IS_FIRST_ENTRY(*lst_hd)){
					is_first_entry = 1;
				}
				(*lst_hd)->pre = &(cblk_p->cbe_hash);
				cblk_p->cbe_hash.next = *lst_hd;
				if(is_first_entry == 1){
					chunk_blk_entry_hash[cblk_hash_v] = &(cblk_p->cbe_hash);
				}
				break;
			case 2:
				(*lst_hd)->next = &(cblk_p->cbe_hash);
				cblk_p->cbe_hash.pre = *lst_hd;
				break;
			default:
				break;
		}
	}
	printf("%d chunk block entrys received!\n",i);
	/* all chunk_block_entryS have been received 
	 * Now find the duplicate block from the src file */
	dup_blk_off = 0;
	dup_blk_len = block_sz;
	delta_region_start = dup_blk_off;
	delta_region_end = dup_blk_off;
	while(dup_blk_off < file_sz){
	}
over3:
	free(cblk_array);
over2:
	close(fd);
over1:
	close(connfd);
	return 0;
}
