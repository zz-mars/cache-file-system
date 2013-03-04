/* cache file system namespace manager 
 * BY grant chen 27,Feb,2013 */
#include"glob.h"
#include"errmsg.h"
/*  two kinds of user defined in cache fs
 * 1) Super user : 
 *			super user of cfs,manages system information.
 * 2) Individual users : 
 *			every individual user has a home dir under /,
 *			for example,user1 has a home dir /user1
 * 3) How to share files in cfs :
 *			All the shared files in cfs can be accessed in dir /shared.
 *			There's a shared dir in every individual user's home dir.
 *			When users enter their shared dir,the actual action is that cwd is changed to /shared/.
 *			Special operations are needed to dispose shared dir.
 *	  */

/* file type
 * 1) REGULAR_FILE : name is file name
 *	     			 parent is the pointer to its parent ns_node 
 *					 child is set to NULL as regular file has no child 
 *					 how_many_children is set to 0 
 * 2) DIRECTORY_FILE : name is directory name
 *					   parent is the pointer to its parent ns_node 
 *					   child is the pointer to a pointer array,whose elements is the pointer to its child 
 *					   how_many_children is the number of all its children 
 * 3) SHARED_FLAG	 : is one part of file type,only make sense for dir file.
 *					   when SHARED_FLAG of dir file is set,it
 * special file type for the "shared" dir file in every individual user's home dir.
 *						This special file is the soft link to /shared.
 */
/* file type definition 
 * A FILE'S TYPE CAN BE */ 
#define REGULAR_FILE		 00
#define DIRECTORY_FILE		 01
/* for every dir whose SHARED_FLAG is set,it is a soft link to /shared.
 * ONLY SUPER USER CAN MAKE THIS KIND OF DIR FILE WITH SHARED_FLAG SET.
 * ONLY /shared 'S SHARED_DIR SET.*/
#define SHARED_FLAG			 02
/* SHARED_DIR is only for /shared dir
 * when going to the UPPER_DIR,if we are now in /shared (OR SHARED_DIR SET),
 * we go to the current user's home dir instead of dir "/" */
#define SHARED_DIR			 04	/* only for /shared */

#define IS_DIR_FILE(file_type)			(file_type & DIRECTORY_FILE)
#define IS_REG_FILE(file_type)			(!(file_type | REGULAR_FILE))
#define IS_SHARED_FLAG_SET(file_type)	(file_type & SHARED_FLAG)
#define IS_SHARED_DIR(file_type)		(file_type & SHARED_DIR)

#define HOME_FOR_SU          "/su"
#define HOME_FOR_SHARED      "/shared"

#define HOME_DIR_NAME		 "~"
#define ROOT_DIR_NAME		 "/"
#define UPPER_DIR_NAME		 ".."
#define SHARED_DIR_NAME	     "shared"
/* user's information is saved in a file */
#define USER_INFO_FILE		"user_info.txt"
/* super user info */
#define SU_UID				00
#define SU_GID				00
#define SU_NAME				"su"
#define SU_INIT_PW			"123"
/* user's info format 
 * uid + gid + home_dir + user_name + password */
#define USER_INFO_FMT				"%d %d \/%s %s %s\n"
#define GET_USER_INFO_CMD_FMT		"grep %s %s"  /*1st %s is user_name,2nd is user_info file */
#define GET_LEAST_AVAILABLE_UID		"./get_least_available_uid.sh"
#define USER_INFO_BUFSZ				1024
/* access control list */
#define UREAD				0400
#define UWRIT				0200
#define UEXEC				0100
#define GREAD				0040
#define GWRIT				0020
#define GEXEC				0010
#define OREAD				0004
#define OWRIT				0002
#define OEXEC				0001
/*-------------------------------------split------------------------------------*/
typedef struct NS_NODE{
	u8 * name;                     /* file name */
	u8 file_type;                  /* REGULAR_FILE || DIRECTORY_FILE || SLINK_FILE */
	u32 uid;
	u32 gid;
	u32 acl;					   /* access control list */
	struct NS_NODE * parent;	   /* parent */
	struct NS_NODE ** child;       /* child array */
	u32 how_many_children;         /* how many children */
}ns_node;
/* cache file system user */
typedef struct CFS_USER{
	u32 uid;
	u32 gid;
	struct CFS_USER * pre;
	struct CFS_USER * next;
}cfs_user;
/* namespace system stat information definition */
/* initialized  on login */
static cfs_user current_user;  /* current user */
static ns_node * current_working_dir; /* current working dir */
static ns_node * home_dir;      /* user's home dir,set when user login */
/* initialized on init_ns */
static ns_node cfs_root_dir; /* ns_node of root dir*/
static ns_node * shared_dir;    /* shared dir */
static cfs_user super_user;
static cfs_user * active_user_list; /* all active user in cfs are in a list */
/* set current_user */
#define SET_CU(uid,gid)		(current_user.uid = uid;current_user.gid = gid;)
#define SET_CWD(cwd)		(current_working_dir = cwd;)

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
	/* return the full path of a given file_name */
	return NULL;
}
/*-------------------------------------split------------------------------------*/
u32 get_ns_node(u8 * file_path,ns_node ** nsnode,u32 * index,u8 * file_name_buf,fbufsiz)
{
	/* get ns_node for a given path.
	 * *index is the file's position in the last lookup,no matter this look up succeeds or fails.
	 * RETURN TABLE :
	 *						return_value	file_name_buf		*nsnode
	 * success					0			 empty				ns_node ptr of the right file(not upper dir file)
	 * lkup_node not a dir :	1			 empty				the ptr of ns_node that leads to error (not a dir)
	 * is upper dir file   :    2			 empty			 	the ptr of the parent ns_node
	 * no such file or dir :	3			 file name			ptr of final dir in this file_path
	 * no such file or dir :	4			 inter_dir name		ptr of the last dir when "no such file or dir" happens
	 * ATTENTION : index only makes sense when return value is 0 : used to delete a file
	 *														   3 : used to make a new file 
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
			/* just to see what happened in last lookup! */
			switch(ret){
				case 0:
					/* everything is just fine! */
					break;
				case 1:
					/* impossible */
					break;
				case 2:
					/* upper dir */
					break;
				case 3:
					/* ENOENT HAPPENS IN LAST LOOKUP,
					 * BUT STILL WE GOT HERE!
					 * SO IT IS A INTER_DIR LOOKUP FAILURE! */
					ret = 4;
					goto op_over;
				default:
					/* unrecognized */
					break;
			}
			/* reset ret */
			ret = 0;
			if(!IS_DIR_FILE(lkup_node->file_type)){
					serrmsg("NOT A DIRECTORY : %s",lkup_node->name);
					ret = 1;
					goto op_over;
			}
			i = fn_tail - fn_head;
			strncpy(file_name_buf,fn_head,i);
			if(strcmp(file_name_buf,UPPER_DIR_NAME) == 0){
				/* go to upper dir */
				if(IS_SHARED_DIR(lkup_node->file_type)){
					/* for /shared dir,UPPER_DIR is the home dir */
					lkup_node = home_dir;
				}else{
					/* just go back to the parent */
					lkup_node = lkup_node->parent;
				}
				/* set ret to tell caller this is a UPPER_DIR */
				ret = 2;
				goto cont;
			}
			/* not UPPER_DIR */
			i = binary_seach_file(file_name_buf,lkup_node->child,0,lkup_node->how_many_children - 1);
			if(strcmp(file_name_buf,lkup_node->child[i]->name) != 0){
				/* look up fail */
				serrmsg("%s NO SUCH FILE OR DIRECTORY UNDER DIRECTORY %s",file_name_buf,lkup_node->name);
				ret = 3;
				/* just continue to see if still there are something in the file_path */
				goto cont;
			}
			/* go to next dir */
			lkup_node = lkup_node->child[i];
		}
cont:
		/* reset lookup */
		if(ret != 3 && IS_DIR_FILE(lkup_node->file_type) && IS_SHARED_FLAG_SET(lkup_node->file_type)){
			/* this is the user's shared dir */
			lkup_node = shared_dir;
		}
		bzero(file_name_buf,fbufsiz);
		fn_head = NULL;
		fn_tail = NULL;
	}
op_over:
	*index = i;
	*nsnode = lkup_node;
	return ret;
}
/*-------------------------------------split------------------------------------*/
ns_node * mkfile(u8 * file_path,u8 file_type,u32 acl)
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
	if(r == 3){
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
		new_file->uid = current_user.uid;
		new_file->gid = current_user.gid;
		new_file->acl = acl;
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
	}
ret:
	return new_file;
}
/*-------------------------------------split------------------------------------*/
u32 rmfile(u8 * file_path)
{
	/* remove file "file_path" */
	ns_node * rmnode,*parent_dir;
	ns_node ** child;
	u32 i,j,k,r;
	u8 file_name_buf[FILE_PATH_LEN];
	bzero(file_name_buf,FILE_PATH_LEN);
	r = get_ns_node(file_path,&rmnode,&i,file_name_buf,FILE_PATH_LEN);
	if(r == 0){
		/* ready to delete file */
		parent_dir = rmnode->parent;
		child = parent_dir->child;
		parent_dir->child = (ns_node **)calloc(--(cwd->how_many_children),sizeof(ns_node *));
		for(j = 0,k=0;j < parent_dir->how_many_children;j++,k++){
			if(k == i){k++;}
			parent_dir->child[j] = child[k];
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
	}else{return 1;}/* get_ns_node fail */
	return 0;
}
/*-------------------------------------split------------------------------------*/
u32 cd(u8 * file_path)
{
	/* get_ns_node return 0 or 2 */
	u32 r,i,ret = 0;
	u8 file_name_buf[FILE_PATH_LEN];
	ns_node * dest_dir;
	bzero(file_name_buf,FILE_PATH_LEN);
	r = get_ns_node(file_path,&dest_dir,&i,file_name_buf,FILE_PATH_LEN);
	switch(r){
		case 0:
			if(dest_dir->file_type != DIRECTORY_FILE){
				ret = 1;
			}else{
				current_working_dir = dest_dir;
			}
			break;
		case 2:
			current_working_dir = dest_dir;
			break;
		default:
			/* cd error */
			break;
	}
	return ret;
}
/*-------------------------------------split------------------------------------*/
u32 ls(u8 * file_path)
{
	/* list file */
	u32 r,i,ret = 0;
	u8 file_name_buf[FILE_PATH_LEN];
	ns_node * lsfile;
	bzero(file_name_buf,FILE_PATH_LEN);
	r = get_ns_node(file_path,&lsfile,&i,file_name_buf,FILE_PATH_LEN);
	switch(r){
		case 0:
		case 2:
		default:
	}
	return ret;
}
/*-------------------------------------split------------------------------------*/
u32 get_user_info_by_name(u8 * user_name,u8 * buf,u32 bufsiz)
{
	/* get user info by name 
	 * RETURN VALUE : 0 on success,user info will be in buf
	 *				  > 0 fail */
	FILE *fp;
	u32 ret = 0;
	u32 i = 0;
	bzero(buf,bufsiz);
	snprintf(buf,bufsiz,GET_USER_INFO_CMD_FMT,user_name,USER_INFO_FILE);
	if((fp = popen(buf,"r")) == NULL){
		fprintf(stderr,"popen fail!\n");
		ret = 1;
		goto op_over;
	}
	bzero(buf,bufsiz);
	while(fgets(buf + i,bufsiz - i,fp) != NULL){i = strlen(buf);}
	pclose(fp);
	if(strlen(buf) == 0){
		ret = 2;
	}
	return ret;
}
/*-------------------------------------split------------------------------------*/
u32 adduser(u8 * user_name,u8 * pw)
{
	/* add a new individual user in cfs 
	 * 1) get a uid 
	 * 2) make home dir 
	 * 3) update user_info.txt */
	FILE *fp;
	u32 ret = 0;
	u32 uid,gid,i = 0;
	u8 buf[USER_INFO_BUFSZ];
	bzero(buf,USER_INFO_BUFSZ);
	/* check if user name already exist */
	if(get_user_info_by_name(user_name,buf,USER_INFO_BUFSZ) != 2){
		/* user already exist or popen error */
		ret = 1;
		goto op_over;
	}
	/* user name is ok! 
	 * now get the least available uid for new user */
	if((fp = popen(GET_LEAST_AVAILABLE_UID,"r")) == NULL){
		ret = 2;
		goto op_over;
	}
	while(fgets(buf + i,USER_INFO_BUFSZ - i,fp) != NULL){i = strlen(buf);}
	pclose(fp);
	uid = atoi_u32(buf);
	gid = uid;
	bzero(buf,USER_INFO_BUFSZ);
	snprintf(buf,USER_INFO_BUFSZ,USER_INFO_FMT,uid,gid,user_name,user_name,pw);
	/* write user information to file */
	if((i = open(USER_INFO_FILE,O_WRONLY)) < 0){
		ret = 3;
		goto op_over;
	}
	lseek(i,0,SEEK_END);
	if(write(i,buf,strlen(buf)) < 0){
		close(i);
		ret = 4;
		goto op_over;
	}
	close(i);
	/* create home dir for new user */
	SET_CU(uid,gid);
	bzero(buf,USER_INFO_BUFSZ);
	snprintf(buf,USER_INFO_BUFSZ,"\/individuals\/%s",user_name);
	mkfile(buf,DIRECTORY_FILE,0760);
	bzero(buf,USER_INFO_BUFSZ);
	snprintf(buf,USER_INFO_BUFSZ,"\/individuals\/%s\/%s",user_name,SHARED_DIR_NAME);
	mkfile(buf,SHARED_DIR_FILE,0700);
	/* done */
op_over:
	return ret;
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
	/* root dir */
	cfs_root_dir.name = ROOT_DIR_NAME;
	cfs_root_dir.file_type = DIRECTORY_FILE;
	cfs_root_dir.parent = &cfs_root_dir;/* parent dir for root is itself */
	cfs_root_dir.child = (struct NS_NODE **)0;
	cfs_root_dir.how_many_children = 0;
	/* super user */
	super_user.uid = SU_UID;
	super_user.gid = SU_GID;
	SET_CU(SU_UID,SU_GID);
	/* super user's information write to file */
	mkfile(HOME_FOR_SU,DIRECTORY_FILE,0777);
	mkfile(HOME_FOR_INDIVIDUALS,DIRECTORY_FILE,0777);
	mkfile(HOME_FOR_SHARED,DIRECTORY_FILE,0777);
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
