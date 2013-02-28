/* my own error message reporter */
#include"glob.h"
u8 err_msg[ERR_MSG_LEN];
void perrmsg(u8 * msg)
{
	printf("%s:%s\n",msg,err_msg);
	return;
}
