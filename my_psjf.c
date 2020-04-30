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
#include <assert.h>
#include "control.h"
#include "my_psjf.h"

int psjf_cmp_st(const void *u, const void *v) {
	if (((Data_psjf*)u)->ps.st < ((Data_psjf*)v)->ps.st) return -1;
	else if (((Data_psjf*)u)->ps.st > ((Data_psjf*)v)->ps.st) return 1;
	else {
		if (((Data_psjf*)u)->id < ((Data_psjf*)v)->id) return -1;
		else if (((Data_psjf*)u)->id > ((Data_psjf*)v)->id) return 1;
		else return 0;
	}
}
int psjf_cmp_time(const void *u, const void *v) {
	if (((Data_psjf*)u)->ps.time < ((Data_psjf*)v)->ps.time) return -1;
	else if (((Data_psjf*)u)->ps.time > ((Data_psjf*)v)->ps.time) return 1;
	else {
		if (((Data_psjf*)u)->id < ((Data_psjf*)v)->id) return -1;
		else if (((Data_psjf*)u)->id > ((Data_psjf*)v)->id) return 1;
		else return 0;
	}
}
void my_psjf(Data *ps, int n) {
	Data_psjf buf[n];
	for (int i = 0; i < n; i++) {
		buf[i].ps = ps[i];
		buf[i].id = i;
	}
	qsort(buf, n, sizeof(Data_psjf), psjf_cmp_st);

	int cmd[5*n], val[5*n], ptr = 0;
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
			while (rr != n - 1 && buf[rr+1].ps.st == curt) {
				rr++;
				cmd[ptr] = buf[rr].id; val[ptr] = 0; ptr++;
			}
		}
		qsort(buf + ll, rr - ll + 1, sizeof(Data_psjf), psjf_cmp_time);
		if (rr != n - 1 && curt + buf[ll].ps.time > buf[rr+1].ps.st) {
			int t = buf[rr+1].ps.st - curt;
			if (t > 0) {
				cmd[ptr] = buf[ll].id; val[ptr] = t; ptr++;
				buf[ll].ps.time -= t;
				curt += t;
			}
			rr++;
			cmd[ptr] = buf[rr].id; val[ptr] = 0; ptr++;
			while (rr != n - 1 && buf[rr+1].ps.st == curt) {
				rr++;
				cmd[ptr] = buf[rr].id; val[ptr] = 0; ptr++;
			}
		} else {
			cmd[ptr] = buf[ll].id; val[ptr] = buf[ll].ps.time; ptr++;
			curt += buf[ll].ps.time;
			ll++;
		}
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
