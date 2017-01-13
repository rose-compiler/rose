/* This is a subset of the in6.c example */
void myfree(void *ptr);

void foo(void){
	shared int *sptr;

	myfree(sptr);   		/* Should generate a warning due to implicit cast of shared to private. */
}
