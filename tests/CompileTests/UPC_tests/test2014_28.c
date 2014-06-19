/* This is the in6.c example */
shared void *shmalloc(unsigned nbytes);
void shfree(shared void *ptr);
void *mymalloc(unsigned nbytes);
void myfree(void *ptr);

void foo(void){
	shared int *sptr;
	int *pptr;
	sptr = shmalloc(57);
	shfree(sptr);
	pptr = mymalloc(69);
	myfree(pptr);

/* Should generate a warning due to implicit cast of shared to private. */
/* myfree(((int *)(((char *)sptr) + MPISMOFFSET))); */
	myfree(sptr);

/* Should not generate a warning as the cast is explicit. */
/* myfree(((int *)((int *)(((char *)sptr) + MPISMOFFSET)))); */
	myfree((int *)sptr);
}
