/* vim: set ts=4 sw=4: */
/* Filename    : lib_sysv_msg.c
 * Description : Message Queue wrapping function
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : Message Queue is very useful to transfer short message between two processes
 * Rel         : 2nd.
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int sysv_msgget(char *tok, key_t msg_fixkey, int user_mode);
int sysv_msgrm(int msg_id);

/* Name : sysv_msgget
 * Desc : init message queue (getting or creation)
 * Argv : char *tok, key_t msg_fixkey, user_mode(octal notation)
 * Ret  : msg ID (if success), -1 (if fail)
 */
int sysv_msgget(char *tok, key_t msg_fixkey, int user_mode)
{
	key_t	msg_key;
	int		msg_id;
	char    buf_err[128];

	if (tok != NULL) {
		if ((msg_key = ftok(tok, 1234)) == -1) {
			return -1;
		}
	} else {
		msg_key = msg_fixkey;
	}
	if ((msg_id = msgget(msg_key, IPC_CREAT|IPC_EXCL|user_mode)) == -1) {
		if (errno == EEXIST) {
			msg_id = msgget(msg_key, 0);
		}
	}
	if (msg_id == -1) {
		strerror_r(errno, buf_err, sizeof(buf_err));
		fprintf(stderr,"FAIL: msgget(%s) [%s:%d]\n", buf_err, __FUNCTION__, __LINE__);
	}

	return msg_id;
}


/* Name : msg_remove
 * Desc : remove message queue
 * Argv : int msg_id
 * Ret  : 0 (if success), -1 (fail)
 */
int sysv_msgrm(int msg_id)
{
	if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
		fprintf(stderr,"FAIL: msgctl [%s:%d]\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return 0;
}
