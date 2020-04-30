extern int _fd, *_time;
typedef struct _Data{
	char name[17];
	int st, time, pid;
} Data;
void make(Data *ps);
void run(Data *ps, int t);
void clear(Data *ps);
