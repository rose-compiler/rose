shared void *shmalloc(unsigned nbytes);

void foo(void){
	shared int *sptr;

 /* BUG: sptr = ((void *)(((char *)(shmalloc(57))) + MPISMOFFSET)); */
	sptr = shmalloc(57);
}
