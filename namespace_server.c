/* cache file system namespace manager 
 * BY grant chen 27,Feb,2013 */
#include"glob.h"
#include"errmsg.h"
/*  two kinds of user defined in cache fs
 * 1) Super user : 
 *			super user of cfs,manages system information.
 * 2) Individual users : 
 *			every individual user has a home dir under /individuals/,
 *		    dir name is the user's name.
 * 3) About shared files in cfs :
 *			All the shared files in cfs can be accessed in dir /shared.
 *			A soft link file whose dest file is dir /shared will be in every individual user's home dir,
 *			so that any individual user can access all the shared files via this soft link file to /shared.
 *	        Mention that dir /shared never store the real files,just soft links to the shared files.
 *		    All users can share their files by getting file's share_flag set,so that a soft links to this file will be under /shared.
 *	  */
#define USER_NUM			 2  /* only two user*/
#define HOME_FOR_SU          "su"
#define HOME_FOR_INDIVIDUALS "individuals"
#define HOME_FOR_SHARED      "shared"
#define FULL_PATH_OF_SHARED  "/shared"

#define INDIVIDUAL_USER_NUM  1024    /* individual users */
#define DIRECTORY_FILE		 00
#define REGULAR_FILE		 01
#define SLINK_FILE			 02
#define ROOT_DIR_NAME		 "/"
#define UPPER_DIR_NAME		 ".."
#define IS_LEGAL_FILE_TYPE(file_type)  ((file_type == DIRECTORY_FILE) || \
				   					    (file_type == REGULAR_FILE)   || \
								        (file_type == SLINK_FILE))
/* namespace node 
 * only regular file & dir file & soft link file  supported 
 * for file_type in REGULAR_FILE & DIRECTORY_FILE & SLINK_FILE
 * 1) REGULAR_FILE : name is file name
 *	     			 parent is the pointer to its parent ns_node 
 *					 child is set to NULL as regular file has no child 
 *					 how_many_children is set to 0 
 * 2) DIRECTORY_FILE : name is directory name
 *					   parent is the pointer to its parent ns_node 
 *					   child is the pointer to a pointer array,whose elements is the pointer to its child 
 *					   how_many_children is the number of all its children 
 * 3) SLINK_FILE : "link_to" is the path of the destination file. 
 */
/*-------------------------------------split------------------------------------*/
typedef struct NS_NODE{
	u8 * name;                     /* file name */
	u8 * link_to;                  /* file linked to,only for slink file */
	u8 file_type;                  /* REGULAR_FILE || DIRECTORY_FILE || SLINK_FILE */
	struct NS_NODE * parent;	   /* parent */
	struct NS_NODE ** child;       /* child array */
	u32 how_many_children;         /* how many children */
	u32 count;                     /* how many times this file be referenced */
}ns_node;
/* cache file system user */
typedef struct CFS_USER{
	u32 uid;
	u32 gid;
}cfs_user;
/* namespace system stat information definition */
static cfs_user all_user[2]; /* root & individuals */
static ns_node cfs_root_dir; /* ns_node of root dir*/
static cfs_user * current_user;  /* current user */
static ns_node * current_working_dir; /* current working dir */

/*-------------------------------------split------------------------------------*/
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
/*-------------------------------------split------------------------------------*/
ns_node * get_ns_node(u8 * file_name)
{
	/* parse file_name 
	 * get ns_node */
	u8 file_name[FILE_PATH_LEN];
	ns_node * lkup_node;
	u32 fn_depth = 0,inword = 0;
	u32 i = strlen(file_name);
	u8 * fn_head = NULL,*fn_tail = NULL,*p = file_name;
	u8 * tail = file_name + i;/* tail points to the one character right after the last byte in file_name */
	/* set the starting node 
	 * 1) root dir if file_name starts with '/'
	 * 2) else current working dir */
	if(*p == '/'){
		lkup_node = &cfs_root_dir;
		inword = 0;
	}else{
		lkup_node = current_working_dir;
		fn_head = p;
		inword = 1;
		fn_depth++;
	}
	for(p = file_name;p <= tail;p++){
		/* if lkup_node is a slink file,
		 * redirect lkup_node to the dest ns_node */
		/*
		 *
		 *     code  here to redirect
		 *
		 *		tough problem..
		 *		to be continued..
		 *		fuck!
		 */
		/*
		if(lkup_node->file_type == SLINK_FILE){

		}*/
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
/*-------------------------------------split------------------------------------*/
u8 * get_full_path(ns_node * cwd,u8 * file_name)
{
	/* get full path for a given file name under cwd */
	ns_node * f = get_ns_node(cwd,file_name);
	u32 full_path_len = strlen(file_name);
}
/*-------------------------------------split------------------------------------*/
ns_node * mkfile(ns_node * cwd,u8 * file_name,u8 file_type)
{ 
	/* make a new file under directory cwd 
	 * RETURN VALUE : ns_node * of new file is returned on success,
	 *				  NULL is returned on failure.  
	 */
	ns_node * new_file;
	u32 file_name_len;
	ns_node ** child = cwd->child;
	u32 i,j;
	if(!IS_LEGAL_FILE_TYPE(file_type)){
		serrmsg("ILLEGAL FILE TYPE!");
		return (ns_node *)0;
	}
	/* new file cannot be ".." */
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
	new_file = (ns_node *)calloc(1,sizeof(ns_node));
	if(new_file == (ns_node *)0){
		serrmsg("CALLOC FAIL!");
		return new_file;
	}
	file_name_len = strlen(file_name);
	new_file->name = calloc(1,file_name_len + 1);
	if(new_file->name == NULL){
		free(new_file);
		serrmsg("CALLOC FAIL!");
		return (ns_node *)0;
	}
	strncpy(new_file->name,file_name,file_name_len);
	new_file->file_type = file_type;
	new_file->parent = cwd;
	new_file->child = (ns_node **)0;
	new_file->how_many_children = 0;
	new_file->count = 0;
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
/*-------------------------------------split------------------------------------*/
ns_node * mkslnfile(ns_node * cwd,u8 * link_to)
{
	/* Make a new soft link file under dir cwd.
	 * Dest file is identified by name "link_to".
	 * Name of new link file would be the full path of file "link_to". 
	 * RETURN VALUE : ns_node * of new soft link file is returned on success,
	 *				  NULL is returned on failure */
	u8 * new_link_file_name = get_full_path(cwd,link_to);
	ns_node * new_link_file = mkfile(cwd,new_link_file_name,SLINK_FILE);
	ns_node * link_file = get_ns_node(link_to);
	link_file->count++;
	return new_link_file;
}
/*-------------------------------------split------------------------------------*/
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
/*-------------------------------------split------------------------------------*/
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
/*-------------------------------------split------------------------------------*/
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
/*-------------------------------------split------------------------------------*/
ns_node * adduser(u8 * user_name)
{
	/* add a new individual user in cfs 
	 * 1) make a home dir for new user under /individuals 
	 * 2) make a soft link file linked to /shared for this user */
}
/*-------------------------------------split------------------------------------*/
/* namespace initialization */
static void init_ns()
{
	/* dirs need to be initialized 
	 * 1) /
	 * 2) /root/
	 * 3) /individuals/
	 * 4) /shared/ 
	 * */
	cfs_root_dir.name = ROOT_DIR_NAME;
	cfs_root_dir.file_type = DIRECTORY_FILE;
	cfs_root_dir.parent = &cfs_root_dir;/* parent dir for root is itself */
	cfs_root_dir.child = (struct NS_NODE **)0;
	cfs_root_dir.how_many_children = 0;
	/* cwd set when user login */
/*	current_working_dir = &cfs_root_dir; */
	if(mkfile(&cfs_root_dir,HOME_FOR_SU,DIRECTORY_FILE) == (ns_node *)0){
		fprintf(stderr,"INIT DIR \/%s FAIL!\n",HOME_FOR_SU);
	}
	if(mkfile(&cfs_root_dir,HOME_FOR_INDIVIDUALS,DIRECTORY_FILE) == (ns_node *)0){
		fprintf(stderr,"INIT DIR \/%s FAIL!\n",HOME_FOR_INDIVIDUALS);
	}
	if(mkfile(&cfs_root_dir,HOME_FOR_SHARED,DIRECTORY_FILE) == (ns_node *)0){
		fprintf(stderr,"INIT DIR \/%s FAIL!\n",HOME_FOR_SHARED);
	}
	return;
}
/*-------------------------------------split------------------------------------*/


	/*----------------------------------------------------------*/
	/*															*/
	/*															*/
	/*             user login module							*/
	/*				to be continue...							*/
	/*															*/
	/*----------------------------------------------------------*/

int main()
{
	return 0;
}
