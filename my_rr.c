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
#include "my_rr.h"

int rr_cmp(const void *u, const void *v) {
	if (((Data_rr*)u)->ps.st < ((Data_rr*)v)->ps.st) return -1;
	else if (((Data_rr*)u)->ps.st > ((Data_rr*)v)->ps.st) return 1;
	else {
		if (((Data_rr*)u)->id < ((Data_rr*)v)->id) return -1;
		else if (((Data_rr*)u)->id > ((Data_rr*)v)->id) return 1;
		else return 0;
	}
}
void my_rr(Data *ps, int n) {
	Data_rr buf[n];
	for (int i = 0; i < n; i++) {
		buf[i].ps = ps[i];
		buf[i].id = i;
	}
	qsort(buf, n, sizeof(Data_rr), rr_cmp);
	int Q[n];
	int ptr = 0, ll = 0, rr = n - 1, num = 0;

	int curt = 0;
	while (ptr != n || num != 0) {
		if (!num) {
			int t = buf[ptr].ps.st - curt;
			curt = t;
			for (int i = 0; i < t; i++) {
				volatile unsigned long j;
				for (j = 0; j < 1000000UL; j++);
			}
			make(&ps[buf[ptr].id]);
			rr++; if (rr == n) rr = 0;
			Q[rr] = ptr;
			ptr++;
			num++;
		}
		int id = Q[ll];
		int t = ps[ buf[id].id ].time;
		if (t > 500) t = 500;
		while (ptr != n && curt + t >= buf[ptr].ps.st) {
			int dt = buf[ptr].ps.st - curt;
			if (dt) {
				run(&ps[buf[id].id], dt);
				t -= dt;
				curt += dt;
			}
			make(&ps[buf[ptr].id]);
			rr++; if (rr == n) rr = 0;
			Q[rr] = ptr;
			ptr++;
			num++;
		}

		if (t) {
			run(&ps[buf[id].id], t);
			curt += t;
		}
		ll++; if (ll == n) ll = 0;
		num--;
		if (ps[buf[id].id].time) {
			rr++; if (rr == n) rr = 0;
			Q[rr] = id;
			num++;
		}
	}
}
