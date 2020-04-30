#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sched.h>
#include <fcntl.h>
#include "control.h"

int _fd, *_time;
void make(Data *ps) {
	int pid = fork();
	if (pid == 0) {
		char s[17];
		sprintf(s, "%d", ps->time);
		struct sched_param param;
		param.sched_priority = 3;
		sched_setparam(0, &param);
		execlp("./process", "./process", ps->name, s, NULL);
	} else {
		ps->pid = pid;
		sched_yield();
	}
}
void run(Data *ps, int t) {
	ps->time -= t;
	*_time = t;
	struct sched_param param;
	param.sched_priority = 3;
	sched_setparam(ps->pid, &param);
	sched_yield();
}
void clear(Data *ps) {
	waitpid(ps->pid, NULL, 0);
}
