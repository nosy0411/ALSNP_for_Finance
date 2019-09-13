/* vim: set ts=4 sw=4: */
/* Filename    : dir_notify.c
 * Description : notify dir change using RT signal.
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

/* RTS Signal handler function */
void chk_rt(int sig, siginfo_t *siginfo_rt, void *data)
{
	printf("[SIGRT] si->si_band (%lx)\n", siginfo_rt->si_band);
}

int main(int argc, char *argv[])
{
	int fd_dir;
	struct sigaction sa_rt;

	if (argc != 2) {
		printf("Usage : %s <dir>\n", argv[0]); return 0;
	}

	sa_rt.sa_sigaction = chk_rt;
	sigemptyset(&sa_rt.sa_mask);
	sa_rt.sa_flags = SA_SIGINFO;
	sigaction(SIGRTMIN, &sa_rt, NULL);	/* signal hander */

	fd_dir = open(argv[1], O_RDONLY); 	/* open directory */
	fcntl(fd_dir, F_SETSIG, SIGRTMIN);	/* set signal no  */
	fcntl(fd_dir, F_NOTIFY, DN_ACCESS|DN_MODIFY|DN_MULTISHOT); /* set dir notification */
	while (1) {
		pause();
	}
	return 0;
}
