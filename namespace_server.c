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
 * 3) How to share files in cfs :
 *			All the shared files in cfs can be accessed in dir /shared.
 *			There's a shared dir in every individual user's home dir.
 *			When users enter their shared dir,the actual action is that cwd is changed to /shared/.
 *			Special operations are needed to deal with this shared dir.
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
				   					    (file_type == REGULAR_FILE))
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
 * 3) ATTENTION : for the shared dir in every single user's home dir,its name is "/shared"
 *				  its file_type is DIRECTORY_FILE
 *				  its parent is user's home dir
 *				  no child
 */
/*-------------------------------------split------------------------------------*/
typedef struct NS_NODE{
	u8 * name;                     /* file name */
	u8 file_type;                  /* REGULAR_FILE || DIRECTORY_FILE || SLINK_FILE */
	struct NS_NODE * parent;	   /* parent */
	struct NS_NODE ** child;       /* child array */
	u32 how_many_children;         /* how many children */
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
u8 * get_full_path(u8 * file_name)
{
	/* get full path for a given file name under cwd */
	ns_node * f = get_ns_node(cwd,file_name);
	u32 full_path_len = strlen(file_name);
}
/*-------------------------------------split------------------------------------*/
u32 get_ns_node(u8 * file_path,ns_node ** nsnode,u32 * index,u8 * file_name_buf,fbufsiz)
{
	/* get ns_node for a given path.
	 * *index is the file's position in the last lookup,no matter this look up succeeds or fails.
	 * RETURN TABLE :
	 *						return_value	*nsnode		file_name_buf	description
	 * success					0			not_null	 empty			everything is ok
	 * lkup_node not a dir :	1			not_null	 empty			not a dir
	 * no such file or dir :	2			not_null	 not_empty		file not exist
	 * no such file or dir :	3			not_null	 not_empty		inter_dir not exist
	 */
	u32 ret = 0;
	ns_node * lkup_node;
	u32 fn_depth = 0,inword = 0;
	u32 i = strlen(file_path);
	u8 * fn_head = NULL,*fn_tail = NULL,*p = file_path;
	u8 * tail = file_path + i;/* tail points to the one character right after the last byte in file_path */
	/* set the starting node 
	 * 1) root dir if file_path starts with '/'
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
	bzero(file_name_buf,fbufsiz);
	for(p = file_path;p <= tail;p++){
		/* NO SLINK_FILE ALLOWED! */
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
		if(fn_head != NULL && fn_tail != NULL && fn_head < fn_tail){
			/* look up file name identified by string from fn_head to fn_tail in lkup_node */
			if(lkup_node->file_type != DIRECTORY_FILE){
				serrmsg("NOT A DIRECTORY : %s",lkup_node->name);
				ret = 1;
				goto op_over;
			}
			i = fn_tail - fn_head;
			strncpy(file_name_buf,fn_head,i);
			if(strcmp(file_name_buf,UPPER_DIR_NAME) == 0){
				/* go to upper dir */
				lkup_node = lkup_node->parent;
				/* reset file name */
				bzero(file_name_buf,fbufsiz);
				fn_head = NULL;
				fn_tail = NULL;
				continue;
			}
			i = binary_seach_file(file_name_buf,lkup_node->child,0,lkup_node->how_many_children - 1);
			*index = i;
			if(strcmp(file_name_buf,lkup_node->child[i]->name) != 0){
				/* look up fail */
				serrmsg("%s NO SUCH FILE OR DIRECTORY UNDER DIRECTORY %s",file_name_buf,lkup_node->name);
				ret = (fn_tail == tail ? 2 : 3);
				goto op_over;
			}
			/* go to next dir */
			lkup_node = lkup_node->child[i];
			/* reset file name */
			bzero(file_name_buf,fbufsiz);
			fn_head = NULL;
			fn_tail = NULL;
		}
	}
op_over:
	*nsnode = lkup_node;
	return ret;
}
/*-------------------------------------split------------------------------------*/
ns_node * mkfile(u8 * file_path,u8 file_type)
{
	/* make a new file */
	u32 index;
	u32 r;
	ns_node * nsnode;
	ns_node * new_file = (ns_node *)0;
	ns_node ** child;
	u8 file_name_buf[FILE_PATH_LEN];
	if(!IS_LEGAL_FILE_TYPE(file_type)){
		serrmsg("ILLEGAL FILE TYPE!");
		goto ret;
	}
	bzero(file_name_buf,FILE_PATH_LEN);
	r = get_ns_node(file_path,&nsnode,&index,file_name_buf,FILE_PATH_LEN);
	switch(r){
		case 0:
			/* get_ns_node ok! file already exists! */
			break;
		case 1:
			/* error situation,lkup_node not a dir */
			break;
		case 2:
			/* ready to make a new file! */
			if(strlen(file_name_buf) == 0 || nsnode == (ns_node *)0){
				/* some error happened */
				serrmsg("NEW FILE_NAME OR ITS PARENT NODE INVLAID!");
				goto ret;
			}
			child = nsnode->child;
			new_file = (ns_node *)calloc(1,sizeof(ns_node));
			if(new_file == (ns_node *)0){
				serrmsg("CALLOC FAIL!");
				goto ret;
			}
			j = strlen(file_name_buf);
			new_file->name = calloc(1,j + 1);
			if(new_file->name == NULL){
				free(new_file);
				serrmsg("CALLOC FAIL!");
				new_file = (ns_node *)0;
				goto ret;
			}
			strncpy(new_file->name,file_name_buf,j);
			new_file->file_type = file_type;
			new_file->parent = nsnode;
			new_file->child = (ns_node **)0;
			new_file->how_many_children = 0;
			child = (ns_node **)realloc(nsnode->child,(++(nsnode->how_many_children)) * sizeof(ns_node *));
			if(child == (ns_node **)0){
				free(new_file->name);
				free(new_file);
				serrmsg("REALLOC FAIL!");
				new_file = (ns_node *)0;
				goto ret;
			}
			cwd->child = child;
			for(j = nsnode->how_many_children - 1;j > i;j--){
				child[j] = child[j-1];
			}
			child[j] = new_file;
			break;
		case 3:
			/* error situation,some inter_dir not exists! */
			break;
		default:
			/* unrecognized return value from get_ns_node */
			break;
	}
ret:
	return new_file;
}
/*-------------------------------------split------------------------------------*/
u32 rmfile(u8 * file_path)
{
	/* remove file "file_path" */
	ns_node * rmnode;
	u32 i;
	u8 file_name_buf[FILE_PATH_LEN];
	bzero(file_name_buf,FILE_PATH_LEN);
	i = get_ns_node(file_path,&rmnode,&i,file_name_buf,FILE_PATH_LEN);
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
u32 cd(u8 * file_name)
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
u32 ls(u8 * file_name)
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
