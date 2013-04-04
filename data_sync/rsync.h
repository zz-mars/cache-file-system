#define FILE_NAME_LEN	128
#define CHUNK_FILE	"chunk.info"
#define DELTA_FILE	"delta.info"
#define BLK_SZ			(4*ONE_K)
#define BLK_DIFF		00
#define BLK_SAME		01
extern void cal_md5(u8 * ptr,u32 len,u8 md5[]);
#define MD5_SZ			16
#define MD5_EQUAL(p1,p2)	(memcmp(p1,p2,MD5_SZ) == 0)
typedef struct _chunk_file_header{
	u32 block_sz;/* blk sz */
	u64 block_nr;/* how many blk */
	u8 fn[FILE_NAME_LEN];/* file to be synced */
}chunk_file_header;
#define CHUNK_FILE_HEAD_SZ		(sizeof(chunk_file_header))
typedef struct _chunk_block_entry{
	u64 offset;
	u32 len;
	u8 md5[MD5_SZ+1];
}chunk_block_entry;
#define CHUNK_BLOCK_ENTRY_SZ	(sizeof(chunk_block_entry))

typedef struct _delta_file_header{
	u64 block_nr;
	u32 last_block_sz;
	u64 last_block_offset;
}delta_file_header;
#define DELTA_FILE_HEADER_SZ	(sizeof(delta_file_header))
typedef struct _delta_block_entry{
	u64 offset;
	u32 old_len;
	u32 new_len;
	u8 embeded;
}delta_block_entry;
#define DELTA_BLOCK_ENTRY_SZ	(sizeof(delta_block_entry))
