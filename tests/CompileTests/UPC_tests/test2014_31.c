/* This is the in9.c example */
shared void *shmalloc(unsigned nbytes);
void myfree(void *ptr);

void foo(void){
	shared int *sptr;
	int *pptr;
	sptr = shmalloc(56);
	pptr = sptr;	/* Should generate a warning as the cast is implicit. */
	myfree(pptr);
}
