typedef struct _Data_rr {
	Data ps;
	int id;
} Data_rr;
int rr_cmp(const void *u, const void *v);
void my_rr(Data *ps, int n);
