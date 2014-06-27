/* This is the in8.c example */
shared void *shmalloc(unsigned nbytes);
void myfree(void *);

void foo(void){
	shared int *sptr;
	int *pptr;
	sptr = shmalloc(56);
	pptr = (int *)sptr;	/* Should not generate a warning as the cast is explicit */
	myfree(pptr);
}
