/* cache file system namespace manager 
 * BY grant chen 27,Feb,2013 */
#include"glob.h"
#define DIRECTORY_FILE   00
#define REGULAR_FILE     01
#define FILE_TYPE_VALIDATION(file_type)  ((file_type == DIRECTORY_FILE) || (file_type == REGULAR_FILE))
typedef struct NS_NODE{
	u8 * name;                     /* file name */
	u8 is_directory;               /* is this a directory or a regular file */
	struct NS_NODE * parent;	   /* parent */
	struct NS_NODE ** child;       /* child array */
	u32 how_many_children;         /* how many children */
}ns_node;
static ns_node cache_fs_root_dir;
/* namespace initialization */
static void init_ns
{
	cache_fs_root_dir.name = NULL;/* name for root directory is not neccessary */
	cache_fs_root_dir.is_directory = DIRECTORY_FILE;
	cache_fs_root_dir.parent = (struct NS_NODE *)0;
	cache_fs_root_dir.child = (struct NS_NODE **)0;
	cache_fs_root_dir.how_many_children = 0;
	return;
}
static u32 binary_seach_file(u8 *file_name,ns_node ** child,u32 low,u32 high)
{
	/* binary search file "file_name" in child array
	 * return value is the right position or the place where this new file should be inserted to */
	u32 mid;
	u32 i;
	if(high <= low){
		return low;
	}
	mid = (high + low)/2;
	if((i = strcmp(file_name,child[mid]->name)) < 0){
		high = mid -1;
	}else if(i == 0){
		/* find! */
		return mid;
	}else{
		low = mid + 1;
	}
	return binary_seach_file(file_name,child,low,high);
}
ns_node * mkfile(ns_node * cwd,u8 * file_name,u8 file_type)
{
	/*make a new file under current directory*/
	u32 j;
	ns_node ** child = cwd->child;
	u32 i = binary_seach_file(file_name,child,0,cwd->how_many_children - 1);
	if((strcmp(file_name,child[i]->name) == 0) || !FILE_TYPE_VALIDATION(file_type)){
		/* 1) file already exist
		 * 2) illegal file_type */
		return (ns_node *)0;
	}/* else new file should be inserted to position i */
	ns_node * new_file = (ns_node *)calloc(1,sizeof(ns_node));
	if(new_file == (ns_node *)0){
		return new_file;
	}
	u32 file_name_len = strlen(file_name);
	new_file->name = calloc(1,file_name_len + 1);
	if(new_file->name == NULL){
		free(new_file);
		return (ns_node *)0;
	}
	strncpy(new_file->name,file_name,file_name_len);
	new_file->is_directory = file_type;
	new_file->parent = cwd;
	new_file->child = (ns_node **)0;
	new_file->how_many_children = 0;
	child = (ns_node **)realloc(cwd->child,(++(cwd->how_many_children)) * sizeof(ns_node *));
	if(child == (ns_node **)0){
		free(new_file->name);
		free(new_file);
		return (ns_node *)0;
	}
	cwd->child = child;
	for(j = cwd->how_many_children - 1;j > i;j--){
		child[j] = child[j-1];
	}
	child[j] = new_file;
	return new_file;
}
int main()
{
	return 0;
}
