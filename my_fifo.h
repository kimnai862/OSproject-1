typedef struct _Data_fifo {
	Data ps;
	int id;
} Data_fifo;
int fifo_cmp(const void *u, const void *v);
void my_fifo(Data *ps, int n);
