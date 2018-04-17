shared void *shmalloc(unsigned nbytes);
void shfree(shared void *ptr);
void free(void *ptr);

void foo(void){
	shared int *sptr;

 /* BUG: sptr = ((void *)(((char *)(shmalloc(57))) + MPISMOFFSET)); */
	sptr = shmalloc(57);

 /* BUG: shfree(((void *)(((char *)((int *)(((char *)sptr) + MPISMOFFSET))) + MPISMOFFSET))); */
	shfree(sptr);
}
