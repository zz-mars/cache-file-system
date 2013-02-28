/* my own error message reporter */
#define ERR_MSG_LEN   1024
u8 err_msg[ERR_MSG_LEN];
void perrmsg(u8 * msg)
{
	printf("%s:%s\n",msg,err_msg);
	return;
}
