/* vim: set ts=4 sw=4: */
/* Filename    : mutex_robust.c
 * Description : apply robust attribute
 * Author      : SunYoung Kim <sunyzero@gmail.com>
 * Notes       : 2nd Ed.
 */
#define _XOPEN_SOURCE 700
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include "stdalsp.h"
#define NUM_THREADS     7

struct thread_arg {
	pthread_t   tid;    /* thread id    */
	int         idx;    /* thread index */
} *t_arg;

pthread_mutex_t		mutex;
pthread_mutexattr_t	mutexattr;

void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);

int main()
{
	int		i, ret;
	t_arg = (struct thread_arg *)calloc(NUM_THREADS, sizeof(struct thread_arg));

	pthread_mutexattr_init(&mutexattr); /* init mutex attribute var */
	pthread_mutexattr_setrobust(&mutexattr, PTHREAD_MUTEX_ROBUST);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_NORMAL); 
	pthread_mutex_init(&mutex, &mutexattr);

	for(i=0; i<NUM_THREADS; i++) {
		t_arg[i].idx = i;
		if ((ret = pthread_create(&t_arg[i].tid, NULL, 
						start_thread, (void *)&t_arg[i]))) {
			pr_err("pthread_create : %s", strerror(ret));
			return 0;
		}
	}
	clean_thread(t_arg);
	return 0;
}

/* Name : start_thread
 * Desc : thread function with message
 * Argv : int idx (child index)
 * Ret  : None
 */
void *start_thread(void *arg)
{
	struct thread_arg *t_arg = (struct thread_arg *)arg;
	int		ret;

	sleep(t_arg->idx);
	if ((ret = pthread_mutex_lock(&mutex))) {
		if (ret == EOWNERDEAD) {
			printf("\tThread:%d : EOWNERDEAD detected\n", t_arg->idx);
			pthread_mutex_consistent(&mutex); /* recover mutex statement */
		} else {
			printf("\tThread:%d : Error : %s\n", t_arg->idx, strerror(ret));
		}
	}
	pr_out("[Thread:%d] holding mutex", t_arg->idx);
	sleep(1);
	sleep(1);
	sleep(1);
	sleep(1);

	pthread_mutex_unlock(&mutex);
	
	if (t_arg->idx == 1) {
		pr_out("[Thread:%d] pthread_cancel(Thread:2)", t_arg->idx);
		sleep(1);
		pthread_cancel( (t_arg+1)->tid );
	}

	return t_arg; /* an alternative func, pthread_exit(t_arg) */
}

/* Name : clean_thread
 * Desc : clean(join) thread
 * Argv : None
 * Ret  : None
 */
void clean_thread(struct thread_arg *t_arg)
{
	int     i;
	struct thread_arg *t_arg_ret;

	for (i=0; i<NUM_THREADS; i++, t_arg++) {
		pthread_join(t_arg->tid, (void **)&t_arg_ret);
		pr_out("pthread_join : %d - %lu", t_arg->idx, t_arg->tid);
	}
}
