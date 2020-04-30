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
#include "my_fifo.h"
#include "my_psjf.h"
#include "my_rr.h"
#include "my_sjf.h"

void bye(char *s) {
	printf("%s\n", s);
	exit(0);
}
int main() {
	_fd = shm_open("exec_time", O_CREAT | O_RDWR, S_IRWXU);
	ftruncate(_fd, sizeof(int));
	_time = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
	*_time = 0;


	cpu_set_t _set;
	CPU_ZERO(&_set);
	CPU_SET(0, &_set);
	if (sched_setaffinity(0, sizeof(_set), &_set) == -1) bye("affinity error");
	struct sched_param param;
	param.sched_priority = 2;
	sched_setscheduler(0, SCHED_FIFO, &param);


	Data ps[20];
	char cmd[7]; int n;
	scanf("%s %d", cmd, &n);
	for (int i = 0; i < n; i++) 
		scanf("%s %d %d", ps[i].name, &ps[i].st, &ps[i].time);
	if (cmd[0] == 'F') my_fifo(ps, n);
	else if (cmd[0] == 'P') my_psjf(ps, n);
	else if (cmd[0] == 'R') my_rr(ps, n);
	else if (cmd[0] == 'S') my_sjf(ps, n);
	else bye("Undefined scheduling policy");

	for (int i = 0; i < n; i++) {
		clear(&ps[i]);
		printf("%s %d\n", ps[i].name, ps[i].pid);
		fflush(stdout);
	}
	shm_unlink("exec_time");
}
