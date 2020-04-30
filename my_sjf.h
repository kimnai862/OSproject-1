typedef struct _Data_sjf {
	Data ps;
	int id;
} Data_sjf;
int sjf_cmp_st(const void *u, const void *v);
int sjf_cmp_time(const void *u, const void *v);
void my_sjf(Data *ps, int n);
