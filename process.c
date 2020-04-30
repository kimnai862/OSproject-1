#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sched.h>

int main(int argc, char **argv) {
	struct sched_param param;
	param.sched_priority = 1;

	int _fd = shm_open("exec_time", O_RDWR, S_IRWXU);
	int *_time = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
	int last_time = atoi(argv[2]);

	struct timespec st, ed;
	clock_gettime(CLOCK_REALTIME, &st);
	sched_setparam(0, &param);
	sched_yield();
	while (last_time > 0) {
		last_time -= *_time;
		for (int i = 0; i < *_time; i++) {
			volatile unsigned long j;
			for (j = 0; j < 1000000UL; j++);
		}
		if (last_time == 0) {
			clock_gettime(CLOCK_REALTIME, &ed);
			char cmd[107];
			sprintf(cmd, "echo [Project1] %d %lu.%09lu, %lu.%09lu > /dev/kmsg", getpid(), st.tv_sec, st.tv_nsec, ed.tv_sec, ed.tv_nsec);
			system(cmd);

		}
		sched_setparam(0, &param);
		sched_yield();
	}
	return 0;
}
