/* cache file system namespace manager 
 * BY grant chen 27,Feb,2013 */
#include"glob.h"
#include"utility.h"
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
/********************* ACL & FILE_TYPE DEFINITION *******************
 * ACL\BUCKET&OBJ		BUCKET								OBJ
 * read					list all objs under this bucjet		read this obj
 * write				create&remove%overwrite file		not defined
 * read_acl				read the bucket acl					read the obj acl
 * write_acl			write the bucket acl				write the obj acl
 * full_control			read&writ&r_acl&w_acl				read&r_acl&w_acl
 * */
#define DIR_BIT				0x1000
#define REG_BIT				0x2000
#define SHR_FLAG_BIT		0x4000
#define SHR_DIR_BIT			0x8000
#define U_READ				0x0800
#define U_WRIT				0x0400
#define U_RACL				0x0200
#define U_WACL				0x0100
#define G_READ				0x0080
#define G_WRIT				0x0040
#define G_RACL				0x0020
#define G_WACL				0x0010
#define O_READ				0x0008
#define O_WRIT				0x0004
#define O_RACL				0x0002
#define O_WACL				0x0001
#define DIR_BIT_SET(ft)		(ft & DIR_BIT)
#define REG_BIT_SET(ft)		(ft & REG_BIT)
#define SHR_FLG_BIT_SET(ft)	(ft & SHR_FLAG_BIT)
#define SHR_DIR_BIT_SET(ft)	(ft & SHR_DIR_BIT)
#define FT_DIR(ft)			(DIR_BIT_SET(ft) && \
							 !REG_BIT_SET(ft) && \
							 !(SHR_FLG_BIT_SET(ft)&&SHR_DIR_BIT_SET(ft)))
#define FT_REG(ft)			(REG_BIT_SET(ft) &&\
							 !DIR_BIT_SET(ft) &&\
							 !SHR_FLG_BIT_SET(ft) &&\
							 !SHR_DIR_BIT_SET(ft))
#define SET_DIR(ft)			ft |= DIR_BIT
#define SET_REG(ft)			ft |= REG_BIT
#define SET_SHR_FLG(ft)		ft |= SHR_FLG_BIT
#define SET_SHR_DIR(ft)		ft |= SHR_DIR_BIT
 
//THE ORIGINAL DEFINITION NOT USED NOW
////  * file type definition 
//// * A FILE'S TYPE CAN BE  
//#define REGULAR_FILE		 00
//#define DIRECTORY_FILE		 01
///* for every dir whose SHARED_FLAG is set,it is a soft link to /shared.
// * ONLY SUPER USER CAN MAKE THIS KIND OF DIR FILE WITH SHARED_FLAG SET.
// * ONLY /shared 'S SHARED_DIR SET.*/
//#define SHARED_FLAG			 02
///* SHARED_DIR is only for /shared dir
// * when going to the UPPER_DIR,if we are now in /shared (OR SHARED_DIR SET),
// * we go to the current user's home dir instead of dir "/" */
//#define SHARED_DIR			 04	/* only for /shared */
//
//#define IS_DIR_FILE(file_type)			(file_type & DIRECTORY_FILE)
//#define IS_REG_FILE(file_type)			(file_type == REGULAR_FILE)
//#define IS_SHARED_FLAG_SET(file_type)	(file_type & SHARED_FLAG)
//#define IS_SHARED_DIR(file_type)		(file_type & SHARED_DIR)
//#define IS_LEGAL_FILE_TYPE(file_type)   (IS_REG_FILE(file_type) ||\
//										 IS_DIR_FILE(file_type) &&\
//										 !IS_SHARED_FLAG_SET(file_type) &&\
//										 !IS_SHARED_DIR(file_type))
//
///* access control list */
//#define UREAD				0400
//#define UWRIT				0200
//#define UEXEC				0100
//#define GREAD				0040
//#define GWRIT				0020
//#define GEXEC				0010
//#define OREAD				0004
//#define OWRIT				0002
//#define OEXEC				0001
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
#define USER_INFO_FMT				"%d %d /%s %s %s\n"
#define GET_USER_INFO_CMD_FMT		"grep %s %s"  /*1st %s is user_name,2nd is user_info file */
#define GET_LEAST_AVAILABLE_UID		"./get_least_available_uid.sh"
#define USER_INFO_BUFSZ				1024
/*-------------------------------------split------------------------------------*/
typedef struct NS_NODE{
	u8 * name;                     /* file name */
	u16 flag;					   /* acl and file type */
//	u8 file_type;                  /* REGULAR_FILE || DIRECTORY_FILE || SLINK_FILE */
	u32 uid;
	u32 gid;
//	u32 acl;					   /* access control list */
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
static inline void set_cu(u32 uid,u32 gid)
{
	current_user.uid = uid;
	current_user.gid = gid;
}
static inline void set_cwd(ns_node * cwd)
{
	current_working_dir = cwd;
}
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
u32 get_ns_node(u8 * file_path,ns_node ** nsnode,u32 * index,u8 * file_name_buf,u32 fbufsiz)
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
	u8 * fn_head = NULL;
	u8 * fn_tail = NULL;
	u8 * p = file_path;
	u8 * tail = file_path + i;/* tail points to the one character right after the last byte in file_path */
	printf("get_ns_node for file %s\n",file_path);
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
	printf("start lookup from dir -- %s\n",lkup_node->name);
	for(p = file_path;p <= tail;p++){
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
			if(ret == 3){
				serrmsg("LOOKUP INTER_DIR FAIL!");
				ret = 4;
				goto op_over;
			}
			ret = 0;
			i = fn_tail - fn_head;
			bzero(file_name_buf,fbufsiz);
			strncpy(file_name_buf,fn_head,i);
			printf("now lookup file -- %s\n",file_name_buf);
		/***************************** lkup_node is not dir ****************************/
			if(!IS_DIR_FILE(lkup_node->file_type)){
				serrmsg("NOT A DIRECTORY : %s",lkup_node->name);
				ret = 1;
				goto op_over;
			}
		/******************** look up file ***********************/
			if(strcmp(file_name_buf,UPPER_DIR_NAME) == 0){
				printf("go to upper\n");
				/* go to upper dir */
				if(IS_SHARED_DIR(lkup_node->file_type)){
					/* for /shared dir,UPPER_DIR is the home dir */
					printf("for shared dir,its upper is home dir\n");
					lkup_node = home_dir;
				}else{
					printf("go to parent\n");
					/* just go back to the parent */
					lkup_node = lkup_node->parent;
				}
				/* set ret to tell caller this is a UPPER_DIR */
				serrmsg("IS UPPER_DIR \n");
				ret = 2;
				goto cont;
			}
	/************************************** NO SUCH FILE OR DIR *******************************************/		
	/**/	if(lkup_node->how_many_children == 0){
	/**/		/* no children */
	/**/		serrmsg("NO CHILD IN LKUPNODE");
	/**/		printf("no child in dir -- %s\n",lkup_node->name);
	/**/		i = 0;
	/**/		ret = 3;
	/**/		goto cont;
	/**/	}
	/**/	i = binary_seach_file(file_name_buf,lkup_node->child,0,lkup_node->how_many_children - 1);
	/**/	if(strcmp(file_name_buf,lkup_node->child[i]->name) != 0){
	/**/		/* look up fail */
	/**/		printf("no such file or dir -- %s\n",file_name_buf);
	/**/		serrmsg("%s NO SUCH FILE OR DIRECTORY UNDER DIRECTORY %s",file_name_buf,lkup_node->name);
	/**/		ret = 3;
	/**/		/* just continue to see if still there are something in the file_path */
	/**/		goto cont;
	/**/	}
	/*******************************************************************************************************/
			/* go to next dir */
			lkup_node = lkup_node->child[i];
cont:
			/* reset lookup */
			if(ret != 3){
				if(IS_DIR_FILE(lkup_node->file_type) && IS_SHARED_FLAG_SET(lkup_node->file_type)){
					/* this is the user's shared dir */
					lkup_node = shared_dir;
				}
				fn_head = NULL;
				fn_tail = NULL;
			}
		}
	}
op_over:
	if(index != NULL){
		*index = i;
	}
	if(nsnode != (ns_node **)0){
		*nsnode = lkup_node;
	}
	return ret;
}
/*-------------------------------------split------------------------------------*/
ns_node * mkfile(u8 * file_path,u8 file_type,u32 acl)
{
	/* make a new file 
	 * ONLY REGULAR FILE AND DIRECTORY FILE
	 * NO SHARED_FLAG OR SHARED_DIR IS ALLOWED HERE!
	 * 1) THE /user/shared DIR WILL BE CREATED WITH SHARED_FLAG SET WHEN A NEW USER IS ADDED TO CFS
	 * 2) /shared DIR WILL BE CREATED WITH SHARED_DIR SET WHEN INITIALIZE SYSTEM */
	u32 index;
	u32 r,j;
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
	printf("MKFIEL : RETURN VALUE FROM GET_NS_NODE  -- %d\n",r);
	if(r == 3){
		printf("new file -- %s will be created under dir %s\n",file_name_buf,nsnode->name);
		/* ready to make a new file! */
		if(strlen(file_name_buf) == 0 || nsnode == (ns_node *)0){
			/* some error happened */
			serrmsg("NEW FILE_NAME OR ITS PARENT NODE INVLAID!");
			goto ret;
		}
		printf("ready to make a new file : %s \n \t\t under dir : %s\n",file_name_buf,nsnode->name);
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
		nsnode->child = child;
		for(j = nsnode->how_many_children - 1;j > index;j--){
			child[j] = child[j-1];
		}
		child[index] = new_file;
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
		parent_dir->child = (ns_node **)calloc(--(parent_dir->how_many_children),sizeof(ns_node *));
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
u32 ls(u8 * file_path,ns_node ** single_file,ns_node *** all_file_in_dir,u32 * how_many_children)
{
	/* list file 
	 * return 0 on fail! */
	u32 r,i,ret = 0;
	u8 file_name_buf[FILE_PATH_LEN];
	ns_node * lsfile;
	bzero(file_name_buf,FILE_PATH_LEN);
	r = get_ns_node(file_path,&lsfile,&i,file_name_buf,FILE_PATH_LEN);
	if(r == 0 || r == 2){
		if(IS_REG_FILE(lsfile->file_type)){
			*single_file = lsfile;
			ret = 1;
		}else if(IS_DIR_FILE(lsfile->file_type)){
			*all_file_in_dir = lsfile->child;
			*how_many_children = lsfile->how_many_children;
			ret = 2;
		}
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
	printf("get_user_info_by_name command_line : %s\n",buf);
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
op_over:
	return ret;
}
/*-------------------------------------split------------------------------------*/
u32 adduser(u8 * user_name,u8 * pw)
{
	/* add a new individual user in cfs 
	 * 1) get a uid 
	 * 2) make home dir 
	 * 3) update user_info.txt */
	ns_node * hd,*user_shared_dir;
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
	set_cu(uid,gid);
	bzero(buf,USER_INFO_BUFSZ);
	snprintf(buf,USER_INFO_BUFSZ,"/%s",user_name);
	hd = mkfile(buf,DIRECTORY_FILE,0760);
	/* make shared dir in user's home dir */
	bzero(buf,USER_INFO_BUFSZ);
	snprintf(buf,USER_INFO_BUFSZ,"/%s/%s",user_name,SHARED_DIR_NAME);
	user_shared_dir = mkfile(buf,DIRECTORY_FILE,0760);
	user_shared_dir->file_type |= SHARED_FLAG;
	/* set current user back to super user 
	 * coz only super user has the right to add a new user */
	set_cu(SU_UID,SU_GID);
op_over:
	return ret;
}
/*-------------------------------------split------------------------------------*/
u32 user_login()
{
	/* set something 
	 * 1) read user_name&pw from stdin for authentication
	 * 2) set current user
	 * 3) set current working dir
	 * 4) set home dir
	 * */
	u32 ret = 0;
	u8 * p,*q;
	u8 * user_name_hint = "user : ";
	u8 * pw_hint = "password : ";
	u8 user_input_buf[USER_INFO_BUFSZ];
	u32 i,j,r,index;
	cfs_user login_user;
	ns_node * login_home_dir;
	u8 user_info_buf[USER_INFO_BUFSZ];
	bzero(user_input_buf,USER_INFO_BUFSZ);
	fputs(user_name_hint,stdout);
	fgets(user_input_buf,USER_INFO_BUFSZ,stdin);
	i = strlen(user_input_buf);
	user_input_buf[i-1] = '\0';
	r = get_user_info_by_name(user_input_buf,user_info_buf,USER_INFO_BUFSZ);
	if(r != 0){
		ret = 1;
		fprintf(stderr,"user not exist!\n");
		goto op_over;
	}
	i = strlen(user_info_buf);
	user_info_buf[--i] = '\0';
	printf("user info : %s\n",user_info_buf);
	/* now user's info are in user_info_buf */
	bzero(user_input_buf,USER_INFO_BUFSZ);
	fputs(pw_hint,stdout);
	fgets(user_input_buf,USER_INFO_BUFSZ,stdin);
	j = strlen(user_input_buf);
	user_input_buf[--j] = '\0';
	if(*(user_info_buf + i - j - 1) == CH_SPACE && strncmp(user_input_buf,user_info_buf + i - j,j) == 0){
		/* authentication ok */
		p = user_info_buf;
		q = p;
		while(*p != CH_SPACE){p++;}
		*p = '\0';
		login_user.uid = atoi_u32(q);
		q = p + 1;
		while(*q == CH_SPACE){q++;}
		p = q;
		while(*p != CH_SPACE){p++;}
		*p = '\0';
		login_user.gid = atoi_u32(q);
		q = p + 1;
		while(*q == CH_SPACE){q++;}
		p = q;
		while(*p != CH_SPACE){p++;}
		*p = '\0';
		bzero(user_input_buf,USER_INFO_BUFSZ);
		r = get_ns_node(q,&login_home_dir,&index,user_input_buf,USER_INFO_BUFSZ);
		if( r != 0 && r != 2){
			ret = 2;
			perrmsg("get_home_dir");
			goto op_over;
		}
		set_cwd(login_home_dir);
		set_cu(login_user.uid,login_user.gid);
		home_dir = login_home_dir;
	}else{
		fprintf(stderr,"INCORRECT PW!\n");
		ret = 3;
		goto op_over;
	}
op_over:
	return ret;
}
/*-------------------------------------split------------------------------------*/
void cfs_shell()
{
	/* called after user login */
}
/*-------------------------------------split------------------------------------*/
void print_ns_node(ns_node * node)
{
	u8 * file_type;
	if(IS_DIR_FILE(node->file_type)){
		file_type = "DIRECTORY_FILE";
	}else if(IS_REG_FILE(node->file_type)){
		file_type = "REGULAR_FILE";
	}
	printf("file_type : %s\n",file_type);
	printf("file_name : %s\n",node->name);
	printf("uid		  : %d\n",node->uid);
	printf("gid		  : %d\n",node->gid);
	printf("acl		  : %o\n",node->acl);
	printf("parent	  : %s\n",node->parent->name);
	printf("child_num : %d\n",node->how_many_children);
	return;
}
/*-------------------------------------split------------------------------------*/
/* namespace initialization */
static void init_ns()
{
	/* dirs need to be initialized 
	 * 1) /
	 * 2) super user
	 * 3) /shared/ 
	 * */
	/* root dir */
	u32 fd;
	u8 buf[USER_INFO_BUFSZ];
	ns_node * newnode;
	printf("init cfs root dir...\n");
	cfs_root_dir.name = ROOT_DIR_NAME;
	cfs_root_dir.file_type = DIRECTORY_FILE;
	cfs_root_dir.uid = SU_UID;
	cfs_root_dir.gid = SU_GID;
	cfs_root_dir.acl = 0700;
	cfs_root_dir.parent = &cfs_root_dir;/* parent dir for root is itself */
	cfs_root_dir.child = (struct NS_NODE **)0;
	cfs_root_dir.how_many_children = 0;
	print_ns_node(&cfs_root_dir);
	printf("init cfs super user...\n");
	/* super user */
	super_user.uid = SU_UID;
	super_user.gid = SU_GID;
	bzero(buf,USER_INFO_BUFSZ);
	snprintf(buf,USER_INFO_BUFSZ,USER_INFO_FMT,SU_UID,SU_GID,SU_NAME,SU_NAME,SU_INIT_PW);
	printf("super user info :\n %s\n",buf);
	if((fd = open(USER_INFO_FILE,O_WRONLY)) < 0){
		return;
	}
	lseek(fd,0,SEEK_END);
	write(fd,buf,strlen(buf));
	close(fd);
	set_cu(SU_UID,SU_GID);
	set_cwd(&cfs_root_dir);
	/* super user's information write to file */
	printf("make home dir for super user...\n");
	if((newnode = mkfile(HOME_FOR_SU,DIRECTORY_FILE,0700)) == (ns_node *)0){
		perrmsg("make home from su");
		return;
	}
	print_ns_node(newnode);
	printf("make shared dir...\n");
	if((newnode = mkfile(HOME_FOR_SHARED,DIRECTORY_FILE,0777)) == (ns_node *)0){
		perrmsg("make shared dir");
		return;
	}
	printf("set SHARED_DIR flag...\n");
	newnode->file_type = newnode->file_type | SHARED_DIR;
	print_ns_node(newnode);
	return;
}
int main()
{
	init_ns();
	if(user_login() == 0){
		/* login success! */
		printf("login ok!\n");
	}
	return 0;
}
