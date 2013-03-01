/* cache file system namespace manager 
 * BY grant chen 27,Feb,2013 */
#include"glob.h"
#include"errmsg.h"
/* only 3 kinds of user in cache fs
 * 1) Root is the super user,system information is stored in /root/ 
 * 2) Individual users are taken as one kind,every individual user has a home dir under /individuals/ 
 * 3) All the shared files in cfs can be accessed in dir /shared,
 *	  Mention that dir /shared never store the real files,just soft links to the shared files.
 *	  All users can share their files by setting shared flag,so that a soft links to this file will be under /shared .
 *	  */
#define USER_ROOT        "root"
#define USER_INDIVIDUALS "individuals"
/*
#define USER_SHARED      "shared"
*/
#define INDIVIDUAL_USER  1024    /* individual users */
#define DIRECTORY_FILE   00
#define REGULAR_FILE     01
#define SLINK_FILE		 02
#define ROOT_DIR_NAME    "/"
#define UPPER_DIR_NAME   ".."
#define FILE_TYPE_VALIDATION(file_type)  ((file_type == DIRECTORY_FILE) || \
										  (file_type == REGULAR_FILE)   || \
										  (file_type == SLINK_FILE))
/* namespace node 
 * only regular file & dir file & soft link file  supported */
typedef struct NS_NODE{
	u8 * name;                     /* file name */
	u8 file_type;                  /* is this a directory or a regular file */
	struct NS_NODE * parent;	   /* parent */
	struct NS_NODE ** child;       /* child array */
	u32 how_many_children;         /* how many children */
}ns_node;
/* cache file system user */
typedef struct CFS_USER{
	u32 uid;
	u32 gid;
	struct CFS_USER * next;
	struct CFS_USER * pre;
}cfs_user;
static ns_node cache_fs_root_dir;
static current_user
static ns_node * current_working_dir;
/* namespace initialization */
static void init_ns()
{
	cache_fs_root_dir.name = ROOT_DIR_NAME;
	cache_fs_root_dir.file_type = DIRECTORY_FILE;
	cache_fs_root_dir.parent = &cache_fs_root_dir;/* parent dir for root is itself */
	cache_fs_root_dir.child = (struct NS_NODE **)0;
	cache_fs_root_dir.how_many_children = 0;
	current_working_dir = &cache_fs_root_dir;
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
ns_node * get_ns_node(u8 * path)
{
	/* parse path 
	 * get ns_node */
	u8 file_name[FILE_PATH_LEN];
	ns_node * lkup_node;
	u32 fn_depth = 0,inword = 0;
	u32 i = strlen(path);
	u8 * fn_head = NULL,*fn_tail = NULL,*p = path;
	u8 * tail = path + i;/* tail points to the one character right after the last byte in path */
	if(*p == '/'){
		lkup_node = &cache_fs_root_dir;
		inword = 0;
	}else{
		lkup_node = current_working_dir;
		inword = 1;
		fn_depth++;
	}
	for(p = path;p <= tail;p++){
		if(inword == 0 && p < tail && *p != '/'){
			/* just right enter word */
			fn_head = p;
			inword = 1;
			fn_depth++;
		}
		/* just right leave word */
		if(inword == 1 && (*p == '/' || p == tail)){
			/* when p == tail,POINTER p cannot be used to reference any data,
			 * just the position of a file name 's tail */
			fn_tail = p;
			inword = 0;
		}
		if(fn_head != NULL && fn_tail != NULL && fn_head <= fn_tail){
			/* look up file name identified by string from fn_head to fn_tail in lkup_node */
			if(lkup_node->file_type != DIRECTORY_FILE){
				serrmsg("NOT A DIRECTORY : %s",lkup_node->name);
				lkup_node = (ns_node *)0;
				break;
			}
			i = fn_tail - fn_head;
			strncpy(file_name,fn_head,i);
			*(file_name + i) = '\0'; 
			if(strcmp(file_name,UPPER_DIR_NAME) == 0){
				/* go to upper dir */
				lkup_node = lkup_node->parent;
				fn_head = NULL;
				fn_tail = NULL;
				continue;
			}
			i = binary_seach_file(file_name,lkup_node->child,0,lkup_node->how_many_children - 1);
			if(strcmp(file_name,lkup_node->child[i]->name) != 0){
				/* look up fail */
				serrmsg("%s NO SUCH FILE OR DIRECTORY UNDER DIRECTORY %s",file_name,lkup_node->name);
				lkup_node = (ns_node *)0;
				break;
			}
			lkup_node = lkup_node->child[i];
			fn_head = NULL;
			fn_tail = NULL;
		}
	}
	return lkup_node;
}
ns_node * mkfile(ns_node * cwd,u8 * file_name,u8 file_type)
{
	/*make a new file under current directory*/
	ns_node ** child = cwd->child;
	u32 i,j;
	if(!FILE_TYPE_VALIDATION(file_type)){
		serrmsg("ILLEGAL FILE TYPE!");
		return (ns_node *)0;
	}
	if(strcmp(file_name,UPPER_DIR_NAME) == 0){
		serrmsg("FILE ALREADY EXIST : %s",file_name);
		return (ns_node *)0;
	}
	i = binary_seach_file(file_name,child,0,cwd->how_many_children - 1);
	if(strcmp(file_name,child[i]->name) == 0){
		/* file already exist */
		serrmsg("FILE ALREADY EXIST : %s",file_name);
		return (ns_node *)0;
	}/* else new file should be inserted to position i */
	ns_node * new_file = (ns_node *)calloc(1,sizeof(ns_node));
	if(new_file == (ns_node *)0){
		serrmsg("MALLOC FAIL!");
		return new_file;
	}
	u32 file_name_len = strlen(file_name);
	new_file->name = calloc(1,file_name_len + 1);
	if(new_file->name == NULL){
		free(new_file);
		serrmsg("MALLOC FAIL!");
		return (ns_node *)0;
	}
	strncpy(new_file->name,file_name,file_name_len);
	new_file->file_type = file_type;
	new_file->parent = cwd;
	new_file->child = (ns_node **)0;
	new_file->how_many_children = 0;
	child = (ns_node **)realloc(cwd->child,(++(cwd->how_many_children)) * sizeof(ns_node *));
	if(child == (ns_node **)0){
		free(new_file->name);
		free(new_file);
		serrmsg("REALLOC FAIL!");
		return (ns_node *)0;
	}
	cwd->child = child;
	for(j = cwd->how_many_children - 1;j > i;j--){
		child[j] = child[j-1];
	}
	child[j] = new_file;
	return new_file;
}
u32 rmfile(ns_node * cwd,u8 * file_name)
{
	/* remove file "file_name" under directory cwd */
	ns_node * rmnode;
	ns_node ** child = cwd->child;
	u32 i,j,k;
	if(strcmp(file_name,UPPER_DIR_NAME) == 0){
		serrmsg("CANNOT DELETE FILE : %s",file_name);
		return 1;
	}
	i = binary_seach_file(file_name,child,0,cwd->how_many_children - 1);
	rmnode = child[i];
	if(strcmp(file_name,rmnode->name) != 0){
		/* file not exist */
		serrmsg("FILE NOT EXIST : %s",file_name);
		return 2;
	}
	cwd->child = (ns_node **)calloc(--(cwd->how_many_children),sizeof(ns_node *));
	for(j = 0,k=0;j < cwd->how_many_children;j++,k++){
		if(k == i){k++;}
		cwd->child[j] = child[k];
	}
	free(child);
	/*----------------------------------------------------------*/
	/*															*/
	/*															*/
	/*        call posix function to remove file here			*/
	/*															*/
	/*															*/
	/*----------------------------------------------------------*/
	free(rmnode->name);
	free(rmnode);
	return 0;
}
u32 changedir(u8 * file_name)
{
	ns_node * cd_wd = get_ns_node(file_name);
	if(cd_wd == (ns_node *)0){
		perrmsg("get_ns_node");
		return 1;
	}
	if(cd_wd->file_type != DIRECTORY_FILE){
		fprintf(stderr,"not a directory!");
		return 2;
	}
	current_working_dir = cd_wd;
	return 0;
}
u32 lsfile(u8 * file_name)
{
	/* the return value can be ns_node* arrary
	 * or something else */
	ns_node * ls_file = get_ns_node(file_name);
	if(ls_file == (ns_node *)0){
		perrmsg("get_ns_node");
		return 1;
	}
	if(ls_file->file_type == REGULAR_FILE){
		/* for regular file,just list this file */
	}else if(ls_file->file_type == DIRECTORY_FILE){
		/* for dir file,list all its children. 
		 * some options can be provided here,
		 * such as -r :recursively list file
		 *		   -a : list all files 
		 *		   -s : list in order
		 *		   etc...*/
	}
	return 0;
}
int main()
{
	return 0;
}
