/* cache file system namespace manager 
 * BY grant chen 27,Feb,2013 */
#include"glob.h"
#define DIRECTORY_FILE   00
#define REGULAR_FILE     01
typedef struct{
	u8 name[FILE_PATH_LEN];/*file name*/
	u8 is_directory;       /*is this a directory or a regular file*/
	u8 * parent;		   /*parent*/
	u8 * next_sibling;	   /*next sibling*/
	u8 * first_child;      /*first child*/
	u32 how_many_children; /*how many children*/
}ns_entry;
int main()
{
	u32 n = sizeof(ns_entry);
	printf(" sizeof(ns_entry) -- %d\n",n);
	return 0;
}
