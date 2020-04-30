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

int fifo_cmp(const void *u, const void *v) {
	if (((Data_fifo*)u)->ps.st < ((Data_fifo*)v)->ps.st) return -1;
	else if (((Data_fifo*)u)->ps.st > ((Data_fifo*)v)->ps.st) return 1;
	else {
		if (((Data_fifo*)u)->id < ((Data_fifo*)v)->id) return -1;
		else if (((Data_fifo*)u)->id > ((Data_fifo*)v)->id) return 1;
		else return 0;
	}
}
void my_fifo(Data *ps, int n) {
	Data_fifo buf[n];
	for (int i = 0; i < n; i++) {
		buf[i].ps = ps[i];
		buf[i].id = i;
	}
	qsort(buf, n, sizeof(Data_fifo), fifo_cmp);
	int cmd[3*n], val[3*n], ptr = 0;
	int curt = 0;
	int ll = 0, rr = -1;
	while (rr != n - 1 || ll <= rr) {
		if (ll > rr) {
			rr++;
			if (buf[ll].ps.st > curt) {
				cmd[ptr] = -1; val[ptr] = buf[ll].ps.st - curt; ptr++;
				curt = buf[ll].ps.st;
			}
			cmd[ptr] = buf[ll].id; val[ptr] = 0; ptr++;
		}
		while (rr != n - 1 && curt + buf[ll].ps.time >= buf[rr+1].ps.st) {
			int t = buf[rr+1].ps.st - curt;
			if (t > 0) {
				cmd[ptr] = buf[ll].id; val[ptr] = t; ptr++;
				buf[ll].ps.time -= t;
				curt += t;
			}
			rr++;
			cmd[ptr] = buf[rr].id; val[ptr] = 0; ptr++;
		}
		if (buf[ll].ps.time) {
			cmd[ptr] = buf[ll].id; val[ptr] = buf[ll].ps.time; ptr++;
			curt += buf[ll].ps.time;
		}
		ll++;
	}
	for (int i = 0; i < ptr; i++) {
		if (cmd[i] == -1) {
			for (int j = 0; j < val[i]; j++) {
				volatile unsigned long k;
				for (k = 0; k < 1000000UL; k++);
			}
		} else if (val[i] == 0) {
			make(&ps[cmd[i]]);
		} else {
			run(&ps[cmd[i]], val[i]);
		}
	}
}
