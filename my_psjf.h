typedef struct _Data_psjf {
	Data ps;
	int id;
} Data_psjf;
int psjf_cmp_st(const void *u, const void *v);
int psjf_cmp_time(const void *u, const void *v);
void my_psjf(Data *ps, int n);
