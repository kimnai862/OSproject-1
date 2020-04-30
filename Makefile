all: main.c control.c process.c my_fifo.c my_sjf.c my_rr.c my_psjf.c
	gcc -o main main.c control.c my_fifo.c my_sjf.c my_rr.c my_psjf.c -lrt -std=c11
	gcc -o process process.c -lrt -std=c11
clear:
	rm - f main process
