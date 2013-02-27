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
static inline u32 FILE_ALREADY_EXIST(ns_node * cwd,u8 * file_name)
{
	/* check if file "file_name" already exists in cwd 
	 * return value : 
	 * 1) -1 if already exist
	 * 2) >=0 if not exist,and this value is the position the new file should be insert to. 
	 *    This is to make sure the files are in alphabetical order */
	ns_node * tmp;
	u32 i = 0,j;
	for(i = 0;i < cwd->how_many_children;i++){
		tmp = (cwd->child)[i];
		j = strcmp(file_name,tmp->name);
		if(j <= 0){
			return (j == 0 ? -1 : i);
		}
	}
	/* if we get here,
	 * we should insert new file to the tail of child array */
	return i;
}
ns_node * mkfile(ns_node * cwd,u8 * file_name,u8 file_type)
{
	/*make a new file under current directory*/
	if(!FILE_TYPE_VALIDATION(file_type)){
		return (ns_node *)0;
	}
	u32 i;
	ns_node * new_file = (ns_node *)calloc(sizeof(ns_node),1);
	u32 file_name_len = strlen(file_name);
	new_file->name = calloc(file_name_len + 1,1);
	strncpy(new_file->name,file_name,file_name_len);
	new_file->is_directory = file_type;
	new_file->parent = cwd;
	new_file->child = (ns_node *)0;
	new_file->how_many_children = 0;
	cwd->child = (ns_node **)realloc(cwd->child,(++(cwd->how_many_children)) * sizeof(ns_node *));
	for(i = 0;i < cwd->how_many_children;i++){

	}
}
int main()
{
	return 0;
}
