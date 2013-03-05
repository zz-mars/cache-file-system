#include"glob.h"
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
#define IS_REG_FILE(file_type)			(file_type == REGULAR_FILE)
#define IS_SHARED_FLAG_SET(file_type)	(file_type & SHARED_FLAG)
#define IS_SHARED_DIR(file_type)		(file_type & SHARED_DIR)
int main()
{
	u8 file_type = DIRECTORY_FILE;
	u32 i;
	for(file_type = 0;file_type < 8;file_type++){
		i = IS_DIR_FILE(file_type);
		printf("i == %d\n",i);
		if(i){printf("is dir\n");}
		i = IS_REG_FILE(file_type);
		printf("i == %d\n",i);
		if(i){printf("is reg\n");}
		i = IS_SHARED_FLAG_SET(file_type);
		printf("i == %d\n",i);
		if(i){printf("shared flag set\n");}
		i = IS_SHARED_DIR(file_type);
		printf("i == %d\n",i);
		if(i){printf("shared dir set\n");}
		printf("-----------------------------\n");
	}
	return 0;
}
